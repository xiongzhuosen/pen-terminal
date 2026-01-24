<template>
  <view class="wrapper">
    <view class="tab-bar">
      <view class="tab-btn" @click="switchPage('index')">首页</view>
      <view class="tab-btn" @click="switchPage('ssh')">SSH终端</view>
      <view class="tab-btn" @click="switchPage('vnc')">VNC远程</view>
      <view class="tab-btn tab-btn-active" @click="switchPage('file')">文件管理</view>
      <view class="tab-btn" @click="switchPage('about')">关于</view>
    </view>
    <view class="page-container" v-if="activePage === 'file'">
      <view class="file-section">
        <!-- 顶部操作栏 -->
        <div class="file-top-bar">
          <input 
            class="input file-path" 
            v-model="currentPath" 
            placeholder="/root/Desktop" 
            @confirm="loadFileList"
            @focus="setActiveInput('path')"
          >
          <button class="btn file-refresh" @click="loadFileList">刷新</button>
          <div class="file-new-group">
            <button class="file-new-btn" @click="showModal('new-file')">新建文件</button>
            <button class="file-new-btn" @click="showModal('new-folder')">新建文件夹</button>
          </div>
        </div>
        <!-- 文件列表 -->
        <div class="file-list">
          <view
            v-for="(file, idx) in fileList"
            :key="idx"
            class="file-item"
            @click="file.type === 'dir' ? enterDir(file) : previewFile(file)"
          >
            <div class="file-info">
              <span class="file-name">{{ file.name }}</span>
              <span class="file-size">
                {{ file.size }} | {{ file.type === 'dir' ? '文件夹' : '文件' }} | {{ file.mtime }}
              </span>
            </div>
            <div class="file-opt-group">
              <view class="file-opt-chmod" @click.stop="showModal('chmod', file)">chmod</view>
              <view class="file-opt-chown" @click.stop="showModal('chown', file)">chown</view>
              <view class="file-opt-lsattr" @click.stop="showModal('lsattr', file)">lsattr</view>
              <view class="file-opt-rename" @click.stop="showModal('rename', file)">重命名</view>
              <view class="file-opt-del" @click.stop="showModal('del', file)">删除</view>
            </div>
          </view>
        </div>
        <!-- 文件预览区 -->
        <view class="card" style="height: 80px; margin-top: 6px;" v-if="previewFileData">
          <md-viewer v-if="previewFileData.type === 'md'" :md-content="previewFileData.content" />
          <text v-else style="font-size: 10px; color: @color-text; padding: 4px; white-space: pre-wrap; word-wrap: break-word;">
            {{ previewFileData.content }}
          </text>
        </view>
      </view>
      <!-- 内置键盘（点击输入框显示） -->
      <virtual-keyboard 
        :show-keyboard="showKeyboard"
        :active-input="activeInput"
        @key-input="handleKeyInput"
      />
      <!-- chmod模态框 -->
      <view class="modal" v-if="modalType === 'chmod'">
        <view class="modal-content">
          <view class="modal-title">chmod 赋权（{{ currentFile.name || '' }}）</view>
          <view class="modal-body">
            <view class="prop-item">
              <view class="prop-label">权限值：</view>
              <input 
                class="input prop-input" 
                v-model="chmodValue" 
                placeholder="755"
                @focus="setActiveInput('chmod-value')"
              >
              <view class="prop-note">r=4 w=2 x=1</view>
            </view>
            <view class="prop-item">
              <view class="prop-label">符号模式：</view>
              <input 
                class="input prop-input" 
                v-model="chmodSymbol" 
                placeholder="u+x"
                @focus="setActiveInput('chmod-symbol')"
              >
              <view class="prop-note">u=用户 g=组 o=其他</view>
            </view>
          </view>
          <view class="modal-footer">
            <view class="modal-btn-confirm" @click="confirmModal">确定</view>
            <view class="modal-btn-cancel" @click="hideModal">取消</view>
          </view>
        </view>
      </view>
      <!-- chown模态框 -->
      <view class="modal" v-if="modalType === 'chown'">
        <view class="modal-content">
          <view class="modal-title">chown 改属主（{{ currentFile.name || '' }}）</view>
          <view class="modal-body">
            <view class="prop-item">
              <view class="prop-label">用户:组：</view>
              <input 
                class="input prop-input" 
                v-model="chownValue" 
                placeholder="root:root"
                @focus="setActiveInput('chown-user')"
              >
              <view class="prop-note">格式：用户:组</view>
            </view>
            <view class="prop-item">
              <view class="prop-label">递归：</view>
              <input 
                class="input prop-input" 
                v-model="chownRecursive" 
                placeholder="y/n" 
                value="n"
                @focus="setActiveInput('chown-recursive')"
              >
              <view class="prop-note">仅文件夹有效</view>
            </view>
          </view>
          <view class="modal-footer">
            <view class="modal-btn-confirm" @click="confirmModal">确定</view>
            <view class="modal-btn-cancel" @click="hideModal">取消</view>
          </view>
        </view>
      </view>
      <!-- lsattr模态框 -->
      <view class="modal" v-if="modalType === 'lsattr'">
        <view class="modal-content">
          <view class="modal-title">lsattr 查看属性（{{ currentFile.name || '' }}）</view>
          <view class="modal-body">
            <view class="prop-item">
              <view class="prop-label">当前属性：</view>
              <span class="prop-input" style="color: @color-success;">-------------e--</span>
              <view class="prop-note">e=扩展 a=追加 i=不可改</view>
            </view>
            <view class="prop-item">
              <view class="prop-label">设置属性：</view>
              <input 
                class="input prop-input" 
                v-model="lsattrValue" 
                placeholder="+a -i"
                @focus="setActiveInput('lsattr-value')"
              >
              <view class="prop-note">+添加 -删除</view>
            </view>
          </view>
          <view class="modal-footer">
            <view class="modal-btn-confirm" @click="confirmModal">确定</view>
            <view class="modal-btn-cancel" @click="hideModal">取消</view>
          </view>
        </view>
      </view>
      <!-- 重命名模态框 -->
      <view class="modal" v-if="modalType === 'rename'">
        <view class="modal-content">
          <view class="modal-title">重命名（{{ currentFile.name || '' }}）</view>
          <view class="modal-body">
            <view class="prop-item">
              <view class="prop-label">新名称：</view>
              <input 
                class="input prop-input" 
                v-model="renameValue" 
                placeholder="新名称"
                @focus="setActiveInput('rename-value')"
              >
              <view class="prop-note">保留后缀，避免重名</view>
            </view>
          </view>
          <view class="modal-footer">
            <view class="modal-btn-confirm" @click="confirmModal">确定</view>
            <view class="modal-btn-cancel" @click="hideModal">取消</view>
          </view>
        </view>
      </view>
      <!-- 删除模态框 -->
      <view class="modal" v-if="modalType === 'del'">
        <view class="modal-content">
          <view class="modal-title">删除（{{ currentFile.name || '' }}）</view>
          <view class="modal-body">
            <view class="prop-item">
              <view class="prop-label">确认：</view>
              <span class="prop-input" style="color: @color-danger;">不可恢复！</span>
              <view class="prop-note">文件夹递归删除</view>
            </view>
          </view>
          <view class="modal-footer">
            <view class="modal-btn-confirm" @click="confirmModal">确定</view>
            <view class="modal-btn-cancel" @click="hideModal">取消</view>
          </view>
        </view>
      </view>
      <!-- 新建文件模态框 -->
      <view class="modal" v-if="modalType === 'new-file'">
        <view class="modal-content">
          <view class="modal-title">新建文件</view>
          <view class="modal-body">
            <view class="prop-item">
              <view class="prop-label">文件名：</view>
              <input 
                class="input prop-input" 
                v-model="newFileName" 
                placeholder="new_file.txt"
                @focus="setActiveInput('new-file-name')"
              >
              <view class="prop-note">支持.txt/.sh/.md等后缀</view>
            </view>
          </view>
          <view class="modal-footer">
            <view class="modal-btn-confirm" @click="confirmModal">确定</view>
            <view class="modal-btn-cancel" @click="hideModal">取消</view>
          </view>
        </view>
      </view>
      <!-- 新建文件夹模态框 -->
      <view class="modal" v-if="modalType === 'new-folder'">
        <view class="modal-content">
          <view class="modal-title">新建文件夹</view>
          <view class="modal-body">
            <view class="prop-item">
              <view class="prop-label">文件夹名：</view>
              <input 
                class="input prop-input" 
                v-model="newFolderName" 
                placeholder="new_folder"
                @focus="setActiveInput('new-folder-name')"
              >
              <view class="prop-note">支持中文，默认权限755</view>
            </view>
          </view>
          <view class="modal-footer">
            <view class="modal-btn-confirm" @click="confirmModal">确定</view>
            <view class="modal-btn-cancel" @click="hideModal">取消</view>
          </view>
        </view>
      </view>
    </view>
  </view>
</template>
<script>
import MdViewer from '@/components/MdViewer.vue';
import VirtualKeyboard from '@/components/VirtualKeyboard.vue';
export default {
  name: "file",
  components: { MdViewer, VirtualKeyboard },
  data() {
    return {
      activePage: "file",
      sshConfig: { host: "", port: "22", user: "root", pass: "" },
      isConnected: false,
      sshConnId: "",
      currentPath: "/",
      fileList: [],
      modalType: "",
      currentFile: {},
      chmodValue: "755",
      chmodSymbol: "",
      chownValue: "root:root",
      chownRecursive: "n",
      lsattrValue: "",
      renameValue: "",
      newFileName: "new_file.txt",
      newFolderName: "new_folder",
      previewFileData: null,
      showKeyboard: false,
      activeInput: ""
    };
  },
  created() {
    this.connectSSH();
  },
  methods: {
    switchPage(page) {
      this.activePage = page;
      $falcon.navTo(page, { from: "file" });
    },
    setActiveInput(inputName) {
      this.activeInput = inputName;
      this.showKeyboard = true;
    },
    handleKeyInput(key) {
      if (key === '\b') {
        // 退格处理
        switch(this.activeInput) {
          case 'path': this.currentPath = this.currentPath.slice(0, -1); break;
          case 'chmod-value': this.chmodValue = this.chmodValue.slice(0, -1); break;
          case 'chmod-symbol': this.chmodSymbol = this.chmodSymbol.slice(0, -1); break;
          case 'chown-user': this.chownValue = this.chownValue.slice(0, -1); break;
          case 'chown-recursive': this.chownRecursive = this.chownRecursive.slice(0, -1); break;
          case 'lsattr-value': this.lsattrValue = this.lsattrValue.slice(0, -1); break;
          case 'rename-value': this.renameValue = this.renameValue.slice(0, -1); break;
          case 'new-file-name': this.newFileName = this.newFileName.slice(0, -1); break;
          case 'new-folder-name': this.newFolderName = this.newFolderName.slice(0, -1); break;
        }
      } else if (key !== '\n' && key !== '\t') {
        // 普通字符输入
        switch(this.activeInput) {
          case 'path': this.currentPath += key; break;
          case 'chmod-value': this.chmodValue += key.replace(/\D/g, ''); break;
          case 'chmod-symbol': this.chmodSymbol += key; break;
          case 'chown-user': this.chownValue += key; break;
          case 'chown-recursive': this.chownRecursive = key.toLowerCase() === 'y' || key.toLowerCase() === 'n' ? key : this.chownRecursive; break;
          case 'lsattr-value': this.lsattrValue += key; break;
          case 'rename-value': this.renameValue += key; break;
          case 'new-file-name': this.newFileName += key; break;
          case 'new-folder-name': this.newFolderName += key; break;
        }
      }
    },
    // 连接SSH（文件管理依赖SSH）
    async connectSSH() {
      try {
        const res = await $falcon.ssh_connect({
          host: this.sshConfig.host || "192.168.1.100",
          port: parseInt(this.sshConfig.port) || 22,
          user: this.sshConfig.user || "root",
          pass: this.sshConfig.pass || ""
        });
        if (res.code === 0) {
          this.isConnected = true;
          this.sshConnId = res.connId;
          this.loadFileList();
        } else {
          $falcon.toast(`SSH连接失败：${res.msg}`);
        }
      } catch (err) {
        $falcon.toast(`连接异常：${err.message}`);
      }
    },
    // 加载文件列表
    async loadFileList() {
      if (!this.isConnected) return;
      const res = await $falcon.ssh_list_files({
        connId: this.sshConnId,
        path: this.currentPath
      });
      if (res.code === 0) {
        this.fileList = res.files.map(file => ({
          name: file.name,
          path: file.path,
          type: file.is_dir ? 'dir' : 'file',
          size: this.formatFileSize(file.size),
          mtime: this.formatTime(file.mtime)
        }));
      } else {
        $falcon.toast(`加载失败：${res.msg}`);
        this.fileList = [];
      }
    },
    // 格式化文件大小
    formatFileSize(bytes) {
      if (bytes === 0) return '--';
      const k = 1024;
      const sizes = ['B', 'KB', 'MB', 'GB'];
      const i = Math.floor(Math.log(bytes) / Math.log(k));
      return parseFloat((bytes / Math.pow(k, i)).toFixed(2)) + ' ' + sizes[i];
    },
    // 格式化时间
    formatTime(timestamp) {
      const date = new Date(timestamp * 1000);
      const year = date.getFullYear();
      const month = (date.getMonth() + 1).toString().padStart(2, '0');
      const day = date.getDate().toString().padStart(2, '0');
      const hour = date.getHours().toString().padStart(2, '0');
      const minute = date.getMinutes().toString().padStart(2, '0');
      return `${year}-${month}-${day} ${hour}:${minute}`;
    },
    // 进入文件夹
    enterDir(file) {
      this.currentPath = file.path.endsWith('/') ? file.path : `${file.path}/`;
      this.loadFileList();
    },
    // 预览文件
    async previewFile(file) {
      this.previewFileData = null;
      try {
        const res = await $falcon.ssh_read_file({
          connId: this.sshConnId,
          path: file.path,
          encoding: 'utf8'
        });
        if (res.code === 0) {
          this.previewFileData = {
            name: file.name,
            type: file.name.endsWith('.md') ? 'md' : 'text',
            content: res.content
          };
        } else {
          $falcon.toast(`读取失败：${res.msg}`);
        }
      } catch (err) {
        $falcon.toast(`读取异常：${err.message}`);
      }
    },
    // 显示模态框
    showModal(type, file = {}) {
      this.modalType = type;
      this.currentFile = file;
      switch (type) {
        case 'chmod':
          this.chmodValue = '755';
          this.chmodSymbol = '';
          break;
        case 'chown':
          this.chownValue = 'root:root';
          this.chownRecursive = 'n';
          break;
        case 'lsattr':
          this.lsattrValue = '';
          break;
        case 'rename':
          this.renameValue = file.name || '';
          break;
        case 'new-file':
          this.newFileName = 'new_file.txt';
          break;
        case 'new-folder':
          this.newFolderName = 'new_folder';
          break;
      }
    },
    // 隐藏模态框
    hideModal() {
      this.modalType = '';
      this.currentFile = {};
      this.showKeyboard = false;
      this.activeInput = '';
    },
    // 确认模态框操作
    async confirmModal() {
      if (!this.isConnected) {
        this.hideModal();
        return;
      }
      const filePath = this.currentFile.path || `${this.currentPath}${this.currentFile.name}`;
      let cmd = '';
      try {
        switch (this.modalType) {
          case 'chmod':
            cmd = this.chmodValue ? `chmod ${this.chmodValue} "${filePath}"` : `chmod ${this.chmodSymbol} "${filePath}"`;
            break;
          case 'chown':
            const recursive = this.chownRecursive === 'y' ? '-R' : '';
            cmd = `chown ${recursive} ${this.chownValue} "${filePath}"`;
            break;
          case 'lsattr':
            cmd = `chattr ${this.lsattrValue} "${filePath}"`;
            break;
          case 'rename':
            const newPath = `${this.currentPath}${this.renameValue}`;
            cmd = `mv "${filePath}" "${newPath}"`;
            break;
          case 'del':
            cmd = this.currentFile.type === 'dir' ? `rm -rf "${filePath}"` : `rm "${filePath}"`;
            break;
          case 'new-file':
            const newFilePath = `${this.currentPath}${this.newFileName}`;
            cmd = `touch "${newFilePath}"`;
            break;
          case 'new-folder':
            const newFolderPath = `${this.currentPath}${this.newFolderName}`;
            cmd = `mkdir -p "${newFolderPath}"`;
            break;
        }
        const res = await $falcon.ssh_exec({
          connId: this.sshConnId,
          command: cmd
        });
        if (res.code === 0) {
          $falcon.toast(`${this.getModalTitle()}成功`);
          this.loadFileList();
          this.hideModal();
        } else {
          $falcon.toast(`${this.getModalTitle()}失败：${res.msg}`);
        }
      } catch (err) {
        $falcon.toast(`${this.getModalTitle()}异常：${err.message}`);
      }
    },
    // 获取模态框标题
    getModalTitle() {
      switch (this.modalType) {
        case 'chmod': return '赋权';
        case 'chown': return '改属主';
        case 'lsattr': return '修改属性';
        case 'rename': return '重命名';
        case 'del': return '删除';
        case 'new-file': return '新建文件';
        case 'new-folder': return '新建文件夹';
        default: return '操作';
      }
    }
  }
};
</script>
<style lang="less" scoped>
@import "../../styles/base.less";
</style>
