<template>
  <view class="keyboard">
    <!-- 第一行：数字+ESC -->
    <view class="key-row">
      <view class="key" @click="sendKey('1')"><text>1</text></view>
      <view class="key" @click="sendKey('2')"><text>2</text></view>
      <view class="key" @click="sendKey('3')"><text>3</text></view>
      <view class="key" @click="sendKey('4')"><text>4</text></view>
      <view class="key" @click="sendKey('5')"><text>5</text></view>
      <view class="key" @click="sendKey('6')"><text>6</text></view>
      <view class="key" @click="sendKey('7')"><text>7</text></view>
      <view class="key" @click="sendKey('8')"><text>8</text></view>
      <view class="key" @click="sendKey('9')"><text>9</text></view>
      <view class="key" @click="sendKey('0')"><text>0</text></view>
      <view class="key key-esc" @click="sendKey('\x1b')"><text>ESC</text></view>
    </view>
    <!-- 第二行：字母行1+TAB -->
    <view class="key-row">
      <view class="key" @click="sendKey('q')"><text>q</text></view>
      <view class="key" @click="sendKey('w')"><text>w</text></view>
      <view class="key" @click="sendKey('e')"><text>e</text></view>
      <view class="key" @click="sendKey('r')"><text>r</text></view>
      <view class="key" @click="sendKey('t')"><text>t</text></view>
      <view class="key" @click="sendKey('y')"><text>y</text></view>
      <view class="key" @click="sendKey('u')"><text>u</text></view>
      <view class="key" @click="sendKey('i')"><text>i</text></view>
      <view class="key" @click="sendKey('o')"><text>o</text></view>
      <view class="key" @click="sendKey('p')"><text>p</text></view>
      <view class="key key-func" @click="sendKey('\t')"><text>TAB</text></view>
    </view>
    <!-- 第三行：字母行2+ENTER -->
    <view class="key-row">
      <view class="key" @click="sendKey('a')"><text>a</text></view>
      <view class="key" @click="sendKey('s')"><text>s</text></view>
      <view class="key" @click="sendKey('d')"><text>d</text></view>
      <view class="key" @click="sendKey('f')"><text>f</text></view>
      <view class="key" @click="sendKey('g')"><text>g</text></view>
      <view class="key" @click="sendKey('h')"><text>h</text></view>
      <view class="key" @click="sendKey('j')"><text>j</text></view>
      <view class="key" @click="sendKey('k')"><text>k</text></view>
      <view class="key" @click="sendKey('l')"><text>l</text></view>
      <view class="key key-func" @click="sendKey('\n')"><text>ENTER</text></view>
    </view>
    <!-- 第四行：字母行3+方向键 -->
    <view class="key-row key-row-last">
      <view class="key" @click="sendKey('z')"><text>z</text></view>
      <view class="key" @click="sendKey('x')"><text>x</text></view>
      <view class="key" @click="sendKey('c')"><text>c</text></view>
      <view class="key" @click="sendKey('v')"><text>v</text></view>
      <view class="key" @click="sendKey('b')"><text>b</text></view>
      <view class="key" @click="sendKey('n')"><text>n</text></view>
      <view class="key" @click="sendKey('m')"><text>m</text></view>
      <view class="key key-arrow" @click="sendKey('\x1b[A')"><text>↑</text></view>
      <view class="key key-arrow" @click="sendKey('\x1b[B')"><text>↓</text></view>
      <view class="key key-arrow" @click="sendKey('\x1b[D')"><text>←</text></view>
      <view class="key key-arrow" @click="sendKey('\x1b[C')"><text>→</text></view>
    </view>
  </view>
</template>

<script>
export default {
  name: "VirtualKeyboard",
  methods: {
    // 发送键盘按键（触发父组件）
    sendKey(key) {
      this.$emit("send-key", key);
    }
  }
};
</script>

<style lang="less" scoped>
@import "../styles/base.less";
</style>
