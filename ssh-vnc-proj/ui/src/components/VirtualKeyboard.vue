<template>
  <div class="keyboard">
    <!-- 字母行 -->
    <div class="key-row">
      <text class="key" @click="pressKey('q')">q</text>
      <text class="key" @click="pressKey('w')">w</text>
      <text class="key" @click="pressKey('e')">e</text>
      <text class="key" @click="pressKey('r')">r</text>
      <text class="key" @click="pressKey('t')">t</text>
      <text class="key" @click="pressKey('y')">y</text>
      <text class="key" @click="pressKey('u')">u</text>
      <text class="key" @click="pressKey('i')">i</text>
      <text class="key" @click="pressKey('o')">o</text>
      <text class="key" @click="pressKey('p')">p</text>
    </div>
    <div class="key-row">
      <text class="key" @click="pressKey('a')">a</text>
      <text class="key" @click="pressKey('s')">s</text>
      <text class="key" @click="pressKey('d')">d</text>
      <text class="key" @click="pressKey('f')">f</text>
      <text class="key" @click="pressKey('g')">g</text>
      <text class="key" @click="pressKey('h')">h</text>
      <text class="key" @click="pressKey('j')">j</text>
      <text class="key" @click="pressKey('k')">k</text>
      <text class="key" @click="pressKey('l')">l</text>
    </div>
    <div class="key-row">
      <text class="key" @click="pressKey('shift')">↑</text>
      <text class="key" @click="pressKey('z')">z</text>
      <text class="key" @click="pressKey('x')">x</text>
      <text class="key" @click="pressKey('c')">c</text>
      <text class="key" @click="pressKey('v')">v</text>
      <text class="key" @click="pressKey('b')">b</text>
      <text class="key" @click="pressKey('n')">n</text>
      <text class="key" @click="pressKey('m')">m</text>
      <text class="key" @click="pressKey('backspace')">←</text>
    </div>
    <div class="key-row">
      <text class="key key-wide" @click="pressKey('ctrl')">Ctrl</text>
      <text class="key key-wide" @click="pressKey('alt')">Alt</text>
      <text class="key key-wide" @click="pressKey('space')">空格</text>
      <text class="key key-wide" @click="pressKey('enter')">回车</text>
    </div>
  </div>
</template>
<script>
export default {
  name: "VirtualKeyboard",
  methods: {
    pressKey(key) { this.$emit("keyPress", key); }
  }
};
</script>
<style lang="less" scoped>
@import "../styles/base.less";
.keyboard {
  width: @screen-w;
  height: auto;
  background-color: #eee;
  padding: @padding-sm;
}
.key-row {
  display: flex;
  justify-content: space-between;
  margin-bottom: @padding-sm;
}
.key {
  width: 20px;
  height: @key-height;
  line-height: @key-height;
  background-color: #fff;
  border: 1px solid @color-border;
  border-radius: @border-radius;
  text-align: center;
  font-size: @key-font-size;
}
.key-wide { width: 40px; }
</style>
