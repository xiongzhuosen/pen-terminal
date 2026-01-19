<template>
  <div class="wrapper">
    <div class="setting-item">
      <span>SSH默认端口:</span>
      <input v-model="sshPort" type="number" class="ipt-set" />
    </div>
    <div class="setting-item">
      <span>VNC分辨率:</span>
      <select v-model="vncSize" class="ipt-set">
        <option value="1024x768">1024x768</option>
        <option value="800x600">800x600</option>
      </select>
    </div>
    <text class="btn success" @click="saveSetting">保存设置</text>
  </div>
</template>

<script>
export default {
  name: 'setting',
  data() {
    return {
      sshPort: 22,
      vncSize: '1024x768'
    }
  },
  methods: {
    onShow() {
      console.log(`设置页面参数: ${JSON.stringify(this.$page.options)}`);
    },
    saveSetting() {
      alert(`✅ 设置保存成功\nSSH端口: ${this.sshPort}\nVNC分辨率: ${this.vncSize}`)
    }
  }
}
</script>

<style lang="less" scoped>
@import "base.less";

.setting-item {
  width: 90%;
  display: flex;
  justify-content: space-between;
  align-items: center;
  margin-bottom: 20px;
  .ipt-set {
    padding: 8px;
    width: 150px;
    #border();
    outline: none;
  }
}
</style>
