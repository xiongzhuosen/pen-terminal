<template>
  <view class="wrapper">
    <!-- 文件路径 -->
    <view class="card">
      <input class="input" v-model="currentPath" placeholder="当前路径（如：/data）" @confirm="loadFileList" />
      <view class="btn" @click="loadFileList">
        <text>刷新</text>
      </view>
    </view>

    <!-- 文件操作栏（chmod/chown/lsattr/删除） -->
    <view class="file-opt">
      <view class="opt-btn chmod" @click="showChmodModal">
        <text>chmod</text>
      </view>
      <view class="opt-btn chown" @click="showChownModal">
        <text>chown</text>
      </view>
      <view class="opt-btn lsattr" @click="showLsattrModal">
        <text>lsattr</text>
      </view>
      <view class="opt-btn del opt-last" @click="showDelModal">
        <text>删除</text>
      </view>
    </view>

    <!-- 文件列表 -->
    <view class="file-list">
      <view 
        v-for="(file, idx) in fileList" 
        :key="idx" 
        class="file-item"
        @click="openFile(file)"
        @touchstart="onFileTouch(idx)"
        @touchend="onFileTouchEnd"
      >
        <text class="file-name">{{ file.name }}</text>
        <text class="file-info">{{ file.size }} KB</text>
      </view>
    </view>

    <!-- 文件查看器（文本/HEX/MD 渲染） -->
    <view class="modal" v-if="showViewer">
      <view class="modal-header">
        <text class="modal-title">{{ currentFileName }}</text>
        <text class="modal-close" @click="closeViewer">×</text>
      </view>
      <view class="modal-content">
        <view style="flex-direction: row; margin-bottom: 8px;">
          <view class="btn" style="margin-right: 4px;" @click="switchView('text')" :class="{ 'btn-active': viewType === 'text' }">
            <text>文本</text>
          </view>
          <view class="btn" style="margin-right: 4px;" @click="switchView('hex')" :class="{ 'btn-active': viewType === 'hex' }">
            <text>HEX</text>
          </view>
          <view class="btn" @click="switchView('md')" :class="{ 'btn-active': viewType === 'md' }">
            <text>MD</text>
          </view>
        </view>
        <view class="viewer-content" :class="{ 'viewer-md': viewType === 'md', 'viewer-hex': viewType === 'hex' }">
          <text v-if="viewType === 'text'">{{ fileText }}</text>
          <text v-else-if="viewType === 'hex'">{{ fileHex }}</text>
          <text v-else-if="viewType === 'md'">{{ fileMd }}</text>
        </view>
      </view>
    </view>

    <!-- chmod 模态框 -->
    <view class="modal" v-if="showChmodModal">
      <view class="modal-header">
        <text class="modal-title">chmod 赋权</text>
        <text class="modal-close" @click="hideChmodModal">×</text>
      </view>
      <view class="modal-content">
        <input class="input" v-model="chmodPerm" placeholder="权限（如：755）" />
        <view class="modal-footer">
          <view class="btn" @click="doChmod">
            <text>确定</text>
          </view>
        </view>
      </view>
    </view>

    <!-- chown 模态框 -->
    <view class="modal" v-if="showChownModal">
      <view class="modal-header">
        <text class="modal-title">chown 改属主</text>
        <text class="modal-close" @click="hideChownModal">×</text>
      </view>
      <view class="modal-content">
        <input class="input" v-model="chownUser" placeholder="用户:组（如：root:root）" />
        <view class="modal-footer">
          <view class="btn" @click="doChown">
            <text>确定</text>
          </view>
        </view>
      </view>
    </view>

    <!-- lsattr 模态框 -->
    <view class="modal" v-if="showLsattrModal">
      <view class="modal-header">
        <text class="modal-title">lsattr 属性</text>
        <text class="modal-close" @click="hideLsattrModal">×</text>
      </view>
      <view class="modal-content">
        <view class="viewer-content">
          <text>{{ lsattrContent }}</text>
        </view>
      </view>
    </view>

    <!-- 删除模态框 -->
    <view class="modal" v-if="showDelModal">
      <view class="modal-header">
        <text class="modal-title">删除文件</text>
        <text class="modal-close" @click="hideDelModal">×</text>
      </view>
      <view class="modal-content">
        <text>确定删除 {{ currentFileName }} 吗？</text>
        <view class="modal-footer">
          <view class="btn" @click="doDel">
            <text>确定</text>
          </view>
        </view>
      </view>
    </view>
  </view>
</template>

<script>
export default {
  name: "FilePage",
  data() {
    return {
      currentPath: "/data",
      fileList: [],
      currentFileName: "",
      currentFilePath: "",
      showViewer: false,
      viewType: "text",
      fileText: "",
      fileHex: "",
      fileMd: "",
      showChmodModal: false,
      chmodPerm: "",
      showChownModal: false,
      chownUser: "",
      showLsattrModal: false,
      lsattrContent: "",
      showDelModal: false,
      activeFileIdx: -1
    };
  },
  onLoad() {
    this.loadFileList();
  },
  methods: {
    // 加载文件列表
    async loadFileList() {
      const res = await $api.file_list(this.currentPath);
      this.fileList = res ? JSON.parse(res) : [];
    },
    // 打开文件（多格式加载）
    async openFile(file) {
      this.currentFileName = file.name;
      this.currentFilePath = `${this.currentPath}/${file.name}`;
      this.showViewer = true;
      this.viewType = "text";
      // 加载文本内容
      this.fileText = await $api.file_read_text(this.currentFilePath);
      // 加载 HEX 内容
      this.fileHex = await $api.file_read_hex(this.currentFilePath);
      // 加载 MD 渲染内容
      this.fileMd = await $api.file_render_md(this.currentFilePath);
    },
    // 关闭查看器
    closeViewer() {
      this.showViewer = false;
      this.currentFileName = "";
      this.currentFilePath = "";
    },
    // 切换查看类型
    switchView(type) {
      this.viewType = type;
    },
    // chmod 赋权
    showChmodModal() {
      if (!this.currentFilePath) { $falcon.toast.show("请先选择文件"); return; }
      this.showChmodModal = true;
      this.chmodPerm = "";
    },
    hideChmodModal() { this.showChmodModal = false; },
    async doChmod() {
      if (!this.chmodPerm) { $falcon.toast.show("权限不能为空"); return; }
      const ret = await $api.file_chmod(this.currentFilePath, this.chmodPerm);
      ret === 0 ? $falcon.toast.show("chmod 成功") : $falcon.toast.show("chmod 失败");
      this.hideChmodModal();
    },
    // chown 改属主
    showChownModal() {
      if (!this.currentFilePath) { $falcon.toast.show("请先选择文件"); return; }
      this.showChownModal = true;
      this.chownUser = "";
    },
    hideChownModal() { this.showChownModal = false; },
    async doChown() {
      if (!this.chownUser) { $falcon.toast.show("用户:组不能为空"); return; }
      const ret = await $api.file_chown(this.currentFilePath, this.chownUser);
      ret === 0 ? $falcon.toast.show("chown 成功") : $falcon.toast.show("chown 失败");
      this.hideChownModal();
    },
    // lsattr 查看属性
    showLsattrModal() {
      if (!this.currentFilePath) { $falcon.toast.show("请先选择文件"); return; }
      this.showLsattrModal = true;
      this.loadLsattr();
    },
    hideLsattrModal() { this.showLsattrModal = false; },
    async loadLsattr() {
      this.lsattrContent = await $api.file_lsattr(this.currentFilePath);
    },
    // 删除文件
    showDelModal() {
      if (!this.currentFilePath) { $falcon.toast.show("请先选择文件"); return; }
      this.showDelModal = true;
    },
    hideDelModal() { this.showDelModal = false; },
    async doDel() {
      const ret = await $api.file_delete(this.currentFilePath);
      if (ret === 0) {
        $falcon.toast.show("删除成功");
        this.hideDelModal();
        this.loadFileList();
        this.closeViewer();
      } else {
        $falcon.toast.show("删除失败");
      }
    },
    // 触摸模拟:active
    onFileTouch(idx) {
      this.activeFileIdx = idx;
      const file = this.fileList[idx];
      this.currentFileName = file.name;
      this.currentFilePath = `${this.currentPath}/${file.name}`;
    },
    onFileTouchEnd() {
      this.activeFileIdx = -1;
    }
  }
};
</script>

<style lang="less" scoped>
@import "../../styles/base.less";
</style>
