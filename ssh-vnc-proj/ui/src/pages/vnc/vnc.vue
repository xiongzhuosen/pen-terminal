<template>
  <view class="wrapper">
    <view class="tab-bar">
      <view class="tab-btn" @click="switchPage('index')">首页</view>
      <view class="tab-btn" @click="switchPage('ssh')">SSH终端</view>
      <view class="tab-btn tab-btn-active" @click="switchPage('vnc')">VNC远程</view>
      <view class="tab-btn" @click="switchPage('file')">文件管理</view>
      <view class="tab-btn" @click="switchPage('about')">关于</view>
    </view>
    <view class="page-container vnc-page" v-if="activePage === 'vnc'">
      <div class="vnc-section">
        <!-- 连接设置区 -->
        <div class="connect-bar">
          <input 
            class="input ip-input" 
            v-model="vncConfig.host" 
            placeholder="VNC 主机：192.168.1.100"
            @focus="setActiveInput('host')"
          >
          <div class="multi-input">
            <input 
              class="input" 
              v-model="vncConfig.port" 
              placeholder="端口：5900" 
              style="flex: 1;"
              @focus="setActiveInput('port')"
            >
            <input 
              class="input" 
              v-model="vncConfig.user" 
              placeholder="用户：root" 
              style="flex: 1;"
              @focus="setActiveInput('user')"
            >
            <input 
              class="input" 
              v-model="vncConfig.pass" 
              placeholder="密码：******" 
              type="password" 
              style="flex: 1;"
              @focus="setActiveInput('pass')"
            >
          </div>
          <div class="connect-btn-group">
            <button class="btn-success" style="flex: 1;" @click="connectVnc" :disabled="isConnected">连接</button>
            <button class="btn-danger" style="flex: 1;" @click="disconnectVnc" :disabled="!isConnected">断开</button>
          </div>
        </div>
        <!-- 状态条 -->
        <div class="status-bar">
          <span class="status-text">● {{ isConnected ? '已连接' : '未连接' }} ({{ vncConfig.host }}:{{ vncConfig.port }})</span>
          <span class="status-subtext">FPS: {{ vncFPS }} | 分辨率：{{ vncWidth }}×{{ vncHeight }} | 色彩：{{ vncColorDepth }}</span>
        </div>
        <!-- VNC画布 -->
        <div class="vnc-canvas" @touchstart="handleTouchStart" @touchmove="handleTouchMove" @touchend="handleTouchEnd">
          <image :src="vncFrame" mode="scaleToFill" style="width: 100%; height: 100%;" />
          <!-- 鼠标指针 -->
          <view
            style="position: absolute; width: 8px; height: 8px; background-color: #fff; border-radius: 50%; border-width: 1px; border-color: #000; border-style: solid; transform: translate(-50%, -50%); pointer-events: none;"
            :style="{ left: mouseX + 'px', top: mouseY + 'px' }"
          ></view>
          <!-- 未连接提示 -->
          <div class="vnc-frame" v-if="!isConnected">
            <div class="placeholder" style="color: #FF9800; font-size: 12px;">
              VNC 远程桌面（104键统一键盘）<br>
              支持IP/端口/密码自定义设置 | RVNC级流畅体验
            </div>
          </div>
        </div>
        <!-- 控制栏 -->
        <div class="vnc-control-bar">
          <div class="vnc-mouse-group">
            <div class="vnc-mouse-btn vnc-mouse-btn-active" @click="setMouseBtn('left')">左键</div>
            <div class="vnc-mouse-btn" @click="setMouseBtn('right')">右键</div>
            <div class="vnc-mouse-btn" @click="setMouseBtn('middle')">中键</div>
          </div>
          <div class="vnc-wheel-group">
            <div class="vnc-wheel-btn" @click="mouseWheel(1)">▲</div>
            <div class="vnc-wheel-btn" @click="mouseWheel(-1)">▼</div>
          </div>
          <div class="vnc-zoom-group">
            <div class="vnc-zoom-btn" @click="zoomOut">-</div>
            <div class="vnc-zoom-btn" @click="zoomIn">+</div>
            <div class="vnc-zoom-btn" @click="resetZoom">1:1</div>
          </div>
          <div class="vnc-func-group">
            <div class="vnc-func-btn vnc-setting" @click="showSetting">设置</div>
            <div class="vnc-func-btn vnc-fullscreen" @click="toggleFullscreen">全屏</div>
            <div class="vnc-func-btn vnc-exit" @click="disconnectVnc">退出</div>
          </div>
        </div>
      </div>
      <!-- 内置键盘（默认显示） -->
      <virtual-keyboard 
        :show-keyboard="true"
        :active-input="activeInput"
        @key-input="handleKeyInput"
      />
      <!-- VNC设置模态框 -->
      <view class="modal" v-if="showSettingModal">
        <view class="modal-content">
          <view class="modal-title">VNC 设置（RVNC级）</view>
          <view class="modal-body">
            <view class="prop-item">
              <view class="prop-label">色彩：</view>
              <input class="input prop-input" v-model="vncColorDepth" placeholder="真彩色/256色/黑白" value="真彩色">
              <view class="prop-note">真彩色画质好</view>
            </view>
            <view class="prop-item">
              <view class="prop-label">帧率：</view>
              <input class="input prop-input" v-model="vncMaxFps" placeholder="30/60/无限制" value="30">
              <view class="prop-note">30帧省性能</view>
            </view>
            <view class="prop-item">
              <view class="prop-label">剪贴板：</view>
              <input class="input prop-input" v-model="vncClipboard" placeholder="开启/关闭" value="开启">
              <view class="prop-note">支持复制粘贴</view>
            </view>
          </view>
          <view class="modal-footer">
            <view class="modal-btn-confirm" @click="saveSetting">保存</view>
            <view class="modal-btn-cancel" @click="hideSetting">取消</view>
          </view>
        </view>
      </view>
    </view>
  </view>
</template>
<script>
import VirtualKeyboard from '@/components/VirtualKeyboard.vue';
export default {
  name: "vnc",
  components: { VirtualKeyboard },
  data() {
    return {
      activePage: "vnc",
      vncConfig: { host: "", port: "5900", user: "root", pass: "" },
      isConnected: false,
      vncConnId: "",
      vncFrame: "",
      vncWidth: 1920,
      vncHeight: 1080,
      vncFPS: 0,
      vncColorDepth: "真彩色",
      vncMaxFps: "30",
      vncClipboard: "开启",
      mouseX: 320,
      mouseY: 130,
      currentMouseBtn: "left",
      scale: 1.0,
      frameTimer: null,
      fpsTimer: null,
      frameCount: 0,
      showSettingModal: false,
      isFullscreen: false,
      activeInput: ""
    };
  },
  beforeDestroy() {
    this.disconnectVnc();
  },
  methods: {
    switchPage(page) {
      this.activePage = page;
      $falcon.navTo(page, { from: "vnc" });
    },
    setActiveInput(inputName) {
      this.activeInput = inputName;
    },
    handleKeyInput(key) {
      switch(this.activeInput) {
        case 'host':
          this.vncConfig.host += key.replace(/\n|\t|\b/g, '');
          break;
        case 'port':
          this.vncConfig.port += key.replace(/\D|\n|\t|\b/g, '');
          break;
        case 'user':
          this.vncConfig.user += key.replace(/\n|\t|\b/g, '');
          break;
        case 'pass':
          this.vncConfig.pass += key.replace(/\n|\t|\b/g, '');
          break;
        default:
          this.sendVncKey(key);
          break;
      }
    },
    setMouseBtn(btn) {
      this.currentMouseBtn = btn;
      document.querySelectorAll('.vnc-mouse-btn').forEach(el => el.classList.remove('vnc-mouse-btn-active'));
      event.target.classList.add('vnc-mouse-btn-active');
    },
    async connectVnc() {
      if (!this.vncConfig.host) {
        $falcon.toast("主机不能为空");
        return;
      }
      try {
        const res = await $falcon.vnc_connect({
          host: this.vncConfig.host,
          port: parseInt(this.vncConfig.port),
          password: this.vncConfig.pass,
          scale: this.scale,
          colorDepth: this.vncColorDepth === "真彩色" ? 24 : this.vncColorDepth === "256色" ? 8 : 1,
          maxFps: parseInt(this.vncMaxFps),
          clipboard: this.vncClipboard === "开启"
        });
        if (res.code === 0) {
          this.isConnected = true;
          this.vncConnId = res.connId;
          this.vncWidth = res.width || 1920;
          this.vncHeight = res.height || 1080;
          this.startFrameUpdate();
          this.startFpsCount();
          $falcon.toast("VNC连接成功（RVNC级体验）");
        } else {
          $falcon.toast(`连接失败：${res.msg}`);
        }
      } catch (err) {
        $falcon.toast(`连接异常：${err.message}`);
      }
    },
    async disconnectVnc() {
      if (!this.isConnected) return;
      try {
        await $falcon.vnc_disconnect(this.vncConnId);
        this.isConnected = false;
        this.vncConnId = "";
        this.vncFrame = "";
        this.vncFPS = 0;
        if (this.frameTimer) clearInterval(this.frameTimer);
        if (this.fpsTimer) clearInterval(this.fpsTimer);
        $falcon.toast("VNC已断开");
      } catch (err) {
        $falcon.toast(`断开失败：${err.message}`);
      }
    },
    startFrameUpdate() {
      this.frameTimer = setInterval(async () => {
        if (!this.isConnected) return;
        const res = await $falcon.vnc_get_frame({
          connId: this.vncConnId,
          scale: this.scale,
          format: "jpeg"
        });
        if (res.code === 0 && res.data) {
          this.vncFrame = `data:image/jpeg;base64,${res.data}`;
          this.frameCount++;
        }
      }, 1000 / parseInt(this.vncMaxFps));
    },
    startFpsCount() {
      this.fpsTimer = setInterval(() => {
        this.vncFPS = this.frameCount;
        this.frameCount = 0;
      }, 1000);
    },
    handleTouchStart(e) {
      const touch = e.touches[0];
      this.mouseX = touch.clientX;
      this.mouseY = touch.clientY;
      this.sendMouseMove();
    },
    handleTouchMove(e) {
      e.preventDefault();
      const touch = e.touches[0];
      this.mouseX = touch.clientX;
      this.mouseY = touch.clientY;
      this.sendMouseMove();
    },
    handleTouchEnd() {
      // 模拟鼠标点击
      $falcon.vnc_mouse_down({
        connId: this.vncConnId,
        button: this.currentMouseBtn
      });
      setTimeout(() => {
        $falcon.vnc_mouse_up({
          connId: this.vncConnId,
          button: this.currentMouseBtn
        });
      }, 50);
    },
    sendMouseMove() {
      if (!this.isConnected) return;
      $falcon.vnc_mouse_move({
        connId: this.vncConnId,
        x: this.mouseX,
        y: this.mouseY,
        scale: this.scale
      });
    },
    mouseWheel(direction) {
      if (!this.isConnected) return;
      $falcon.vnc_mouse_wheel({
        connId: this.vncConnId,
        direction: direction,
        amount: 3,
        x: this.mouseX,
        y: this.mouseY
      });
    },
    zoomIn() {
      this.scale = Math.min(2.0, this.scale + 0.1);
      this.updateScale();
    },
    zoomOut() {
      this.scale = Math.max(0.5, this.scale - 0.1);
      this.updateScale();
    },
    resetZoom() {
      this.scale = 1.0;
      this.updateScale();
    },
    async updateScale() {
      if (!this.isConnected) return;
      await $falcon.vnc_set_scale({
        connId: this.vncConnId,
        scale: this.scale
      });
      $falcon.toast(`缩放比例：${this.scale.toFixed(1)}x`);
    },
    toggleFullscreen() {
      $falcon.screen_set_fullscreen({
        fullscreen: !this.isFullscreen,
        orientation: "landscape"
      });
      this.isFullscreen = !this.isFullscreen;
      $falcon.toast(this.isFullscreen ? "已进入全屏" : "已退出全屏");
    },
    showSetting() {
      this.showSettingModal = true;
    },
    hideSetting() {
      this.showSettingModal = false;
    },
    async saveSetting() {
      if (!this.isConnected) {
        this.hideSetting();
        return;
      }
      await $falcon.vnc_set_params({
        connId: this.vncConnId,
        colorDepth: this.vncColorDepth === "真彩色" ? 24 : this.vncColorDepth === "256色" ? 8 : 1,
        maxFps: parseInt(this.vncMaxFps),
        clipboard: this.vncClipboard === "开启"
      });
      $falcon.toast("VNC设置保存成功");
      this.hideSetting();
      // 重启帧更新以应用新帧率
      if (this.frameTimer) clearInterval(this.frameTimer);
      this.startFrameUpdate();
    },
    async sendVncKey(key) {
      if (!this.isConnected) return;
      // 发送键盘按下事件
      await $falcon.vnc_send_key({
        connId: this.vncConnId,
        key: key,
        down: true
      });
      // 发送键盘松开事件
      setTimeout(async () => {
        await $falcon.vnc_send_key({
          connId: this.vncConnId,
          key: key,
          down: false
        });
      }, 50);
    }
  }
};
</script>
<style lang="less" scoped>
@import "../../styles/base.less";
</style>
