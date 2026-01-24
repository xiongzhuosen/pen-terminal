<template>
  <view class="keyboard-wrapper" v-if="showKeyboard">
    <!-- 功能键行 -->
    <div class="kb-row">
      <div class="kb-key kb-esc" @click="sendKey('ESC')">ESC</div>
      <div class="kb-key kb-func" @click="sendKey('F1')">F1</div><div class="kb-key kb-func" @click="sendKey('F2')">F2</div><div class="kb-key kb-func" @click="sendKey('F3')">F3</div><div class="kb-key kb-func" @click="sendKey('F4')">F4</div>
      <div class="kb-key kb-func" @click="sendKey('F5')">F5</div><div class="kb-key kb-func" @click="sendKey('F6')">F6</div><div class="kb-key kb-func" @click="sendKey('F7')">F7</div><div class="kb-key kb-func" @click="sendKey('F8')">F8</div>
      <div class="kb-key kb-func" @click="sendKey('F9')">F9</div><div class="kb-key kb-func" @click="sendKey('F10')">F10</div><div class="kb-key kb-func" @click="sendKey('F11')">F11</div><div class="kb-key kb-func" @click="sendKey('F12')">F12</div>
      <div class="kb-key kb-print" @click="sendKey('Print')">Print</div><div class="kb-key kb-scroll" @click="sendKey('Scroll')">Scroll</div><div class="kb-key kb-pause" @click="sendKey('Pause')">Pause</div>
    </div>
    <!-- 数字行1 -->
    <div class="kb-row">
      <div class="kb-key" @click="sendKey('`')">`</div><div class="kb-key" @click="sendKey('1')">1</div><div class="kb-key" @click="sendKey('2')">2</div><div class="kb-key" @click="sendKey('3')">3</div><div class="kb-key" @click="sendKey('4')">4</div><div class="kb-key" @click="sendKey('5')">5</div>
      <div class="kb-key" @click="sendKey('6')">6</div><div class="kb-key" @click="sendKey('7')">7</div><div class="kb-key" @click="sendKey('8')">8</div><div class="kb-key" @click="sendKey('9')">9</div><div class="kb-key" @click="sendKey('0')">0</div>
      <div class="kb-key" @click="sendKey('-')">-</div><div class="kb-key" @click="sendKey('=')">=</div><div class="kb-key kb-backspace" @click="sendKey('\b')">←</div>
    </div>
    <!-- 数字行2（上档） -->
    <div class="kb-row">
      <div class="kb-key" @click="sendKey('~')">~</div><div class="kb-key" @click="sendKey('!')">!</div><div class="kb-key" @click="sendKey('@')">@</div><div class="kb-key" @click="sendKey('#')">#</div><div class="kb-key" @click="sendKey('$')">$</div><div class="kb-key" @click="sendKey('%')">%</div>
      <div class="kb-key" @click="sendKey('^')">^</div><div class="kb-key" @click="sendKey('&')">&</div><div class="kb-key" @click="sendKey('*')">*</div><div class="kb-key" @click="sendKey('(')">(</div><div class="kb-key" @click="sendKey(')')">)</div>
      <div class="kb-key" @click="sendKey('_')">_</div><div class="kb-key" @click="sendKey('+')">+</div>
    </div>
    <!-- QWERTY行1 -->
    <div class="kb-row">
      <div class="kb-key kb-tab" @click="sendKey('\t')">TAB</div>
      <div class="kb-key" @click="sendKey('Q')">Q</div><div class="kb-key" @click="sendKey('W')">W</div><div class="kb-key" @click="sendKey('E')">E</div><div class="kb-key" @click="sendKey('R')">R</div><div class="kb-key" @click="sendKey('T')">T</div>
      <div class="kb-key" @click="sendKey('Y')">Y</div><div class="kb-key" @click="sendKey('U')">U</div><div class="kb-key" @click="sendKey('I')">I</div><div class="kb-key" @click="sendKey('O')">O</div><div class="kb-key" @click="sendKey('P')">P</div>
      <div class="kb-key" @click="sendKey('[')">[</div><div class="kb-key" @click="sendKey(']')">]</div><div class="kb-key" @click="sendKey('\\')">\</div>
    </div>
    <!-- QWERTY行2（上档） -->
    <div class="kb-row">
      <div class="kb-key" @click="sendKey('{')">{</div><div class="kb-key" @click="sendKey('}')">}</div><div class="kb-key" @click="sendKey('|')">|</div>
      <div class="kb-key" @click="sendKey('A')">A</div><div class="kb-key" @click="sendKey('S')">S</div><div class="kb-key" @click="sendKey('D')">D</div><div class="kb-key" @click="sendKey('F')">F</div><div class="kb-key" @click="sendKey('G')">G</div>
      <div class="kb-key" @click="sendKey('H')">H</div><div class="kb-key" @click="sendKey('J')">J</div><div class="kb-key" @click="sendKey('K')">K</div><div class="kb-key" @click="sendKey('L')">L</div>
      <div class="kb-key" @click="sendKey(';')">;</div><div class="kb-key" @click="sendKey('\'')">'</div>
    </div>
    <!-- QWERTY行3 -->
    <div class="kb-row">
      <div class="kb-key kb-caps" @click="toggleCapsLock()">Caps</div>
      <div class="kb-key" @click="sendKey(':')">:</div><div class="kb-key" @click="sendKey('"')">"</div>
      <div class="kb-key" @click="sendKey('Z')">Z</div><div class="kb-key" @click="sendKey('X')">X</div><div class="kb-key" @click="sendKey('C')">C</div><div class="kb-key" @click="sendKey('V')">V</div><div class="kb-key" @click="sendKey('B')">B</div>
      <div class="kb-key" @click="sendKey('N')">N</div><div class="kb-key" @click="sendKey('M')">M</div>
      <div class="kb-key" @click="sendKey(',')">,</div><div class="kb-key" @click="sendKey('.')">.</div><div class="kb-key" @click="sendKey('/')">/</div>
      <div class="kb-key kb-enter" @click="sendKey('\n')">Enter</div>
    </div>
    <!-- 上档行 -->
    <div class="kb-row">
      <div class="kb-key" @click="sendKey('<')"><</div><div class="kb-key" @click="sendKey('>')">></div><div class="kb-key" @click="sendKey('?')">?</div>
      <div class="kb-key kb-shift" @click="toggleShift()">Shift</div>
      <div class="kb-key kb-shift" @click="toggleShift()">Shift</div>
    </div>
    <!-- 功能键行 -->
    <div class="kb-row">
      <div class="kb-key kb-ctrl" @click="sendKey('Ctrl+')">Ctrl</div><div class="kb-key kb-win" @click="sendKey('Win+')">Win</div><div class="kb-key kb-alt" @click="sendKey('Alt+')">Alt</div>
      <div class="kb-key kb-space" @click="sendKey(' ')">Space</div>
      <div class="kb-key kb-alt" @click="sendKey('Alt+')">Alt</div><div class="kb-key kb-menu" @click="sendKey('Menu')">Menu</div><div class="kb-key kb-ctrl" @click="sendKey('Ctrl+')">Ctrl</div>
    </div>
    <!-- 编辑键行 -->
    <div class="kb-row">
      <div class="kb-key kb-edit" @click="sendKey('Ins')">Ins</div><div class="kb-key kb-edit" @click="sendKey('Del')">Del</div>
      <div class="kb-key kb-edit" @click="sendKey('Home')">Home</div><div class="kb-key kb-edit" @click="sendKey('End')">End</div>
      <div class="kb-key kb-edit" @click="sendKey('PgUp')">PgUp</div><div class="kb-key kb-edit" @click="sendKey('PgDn')">PgDn</div>
      <div class="kb-key kb-arrow" @click="sendKey('↑')">↑</div><div class="kb-key kb-arrow" @click="sendKey('↓')">↓</div><div class="kb-key kb-arrow" @click="sendKey('←')">←</div><div class="kb-key kb-arrow" @click="sendKey('→')">→</div>
    </div>
    <!-- 小键盘行 -->
    <div class="kb-row">
      <div class="kb-key kb-num-lock" @click="toggleNumLock()">NumLock</div>
      <div class="kb-key kb-num" @click="sendKey('7')">7</div><div class="kb-key kb-num" @click="sendKey('8')">8</div><div class="kb-key kb-num" @click="sendKey('9')">9</div>
      <div class="kb-key kb-num-op" @click="sendKey('/')">/</div><div class="kb-key kb-num-op" @click="sendKey('*')">*</div><div class="kb-key kb-num-op" @click="sendKey('-')">-</div>
      <div class="kb-key kb-num" @click="sendKey('4')">4</div><div class="kb-key kb-num" @click="sendKey('5')">5</div><div class="kb-key kb-num" @click="sendKey('6')">6</div>
      <div class="kb-key kb-num" @click="sendKey('1')">1</div><div class="kb-key kb-num" @click="sendKey('2')">2</div><div class="kb-key kb-num" @click="sendKey('3')">3</div><div class="kb-key kb-num" @click="sendKey('0')">0</div>
      <div class="kb-key kb-num" @click="sendKey('.')">.</div><div class="kb-key kb-num-enter" @click="sendKey('\n')">Enter</div><div class="kb-key kb-num-op" @click="sendKey('+')">+</div>
    </div>
  </view>
</template>
<script>
export default {
  name: "VirtualKeyboard",
  props: {
    showKeyboard: { type: Boolean, default: false },
    activeInput: { type: String, default: "" }
  },
  data() {
    return {
      isCapsLock: false,
      isShift: false,
      isNumLock: false
    };
  },
  methods: {
    sendKey(key) {
      // 处理大小写切换
      if (this.isCapsLock ^ this.isShift && /^[A-Z]$/.test(key)) {
        key = key.toLowerCase();
      }
      // 处理Shift组合符号
      const shiftMap = {
        '`': '~', '1': '!', '2': '@', '3': '#', '4': '$', '5': '%',
        '6': '^', '7': '&', '8': '*', '9': '(', '0': ')', '-': '_', '=': '+',
        '[': '{', ']': '}', '\\': '|', ';': ':', '\'': '"', ',': '<', '.': '>', '/': '?'
      };
      if (this.isShift && shiftMap[key]) {
        key = shiftMap[key];
      }
      this.$emit("key-input", key);
    },
    toggleCapsLock() {
      this.isCapsLock = !this.isCapsLock;
      const capsBtn = document.querySelector('.kb-caps');
      capsBtn.style.backgroundColor = this.isCapsLock ? '#FF5722' : '#FF9800';
    },
    toggleShift() {
      this.isShift = !this.isShift;
      document.querySelectorAll('.kb-shift').forEach(btn => {
        btn.style.backgroundColor = this.isShift ? '#FF5722' : '#2196F3';
      });
    },
    toggleNumLock() {
      this.isNumLock = !this.isNumLock;
      const numLockBtn = document.querySelector('.kb-num-lock');
      numLockBtn.style.backgroundColor = this.isNumLock ? '#FF5722' : '#FF9800';
    }
  }
};
</script>
<style lang="less" scoped>
@import "../styles/base.less";
</style>
