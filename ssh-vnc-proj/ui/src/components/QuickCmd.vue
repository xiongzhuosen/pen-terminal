<template>
  <div class="quick-cmd-box">
    <div class="flex-row flex-wrap justify-between">
      <div 
        class="cmd-item" 
        v-for="(item, index) in cmdList" 
        :key="index" 
        @click="execCmd(item.cmd)"
      >
        <text class="cmd-text">{{ item.name }}</text>
      </div>
    </div>
  </div>
</template>

<script>
export default {
  name: 'QuickCmd',
  props: {
    cmdList: {
      type: Array,
      default: () => [
        { name: '查看IP', cmd: 'ifconfig' },
        { name: '磁盘占用', cmd: 'df -h' },
        { name: '内存占用', cmd: 'free -m' },
        { name: '查看权限', cmd: 'ls -l' },
        { name: '赋权777', cmd: 'chmod 777' },
        { name: '清屏', cmd: 'clear' }
      ]
    }
  },
  methods: {
    execCmd(cmd) {
      this.$emit('exec-cmd', cmd)
    }
  }
}
</script>

<style lang="less" scoped>
@import "../styles/base.less";

.flex-row {
  display: flex;
  flex-direction: row;
}

.flex-wrap {
  flex-wrap: wrap;
}

.justify-between {
  justify-content: space-between;
}
</style>
