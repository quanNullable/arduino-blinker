/* *****************************************************************

  BY QUAN

 * *****************************************************************/

#define BLINKER_WIFI
#define BLINKER_ESP_SMARTCONFIG//智能配网
#define BLINKER_MIOT_OUTLET//支持小爱同学 插座模式
#include <Blinker.h>

char auth[] = "b5c77753b0a9";

int GPIO = 0; //定义继电器输入引脚为GPIO/0
bool switchOn = false;//开关状态

// 新建组件对象
BlinkerButton Button1("btn-switch");

// 按下BlinkerAPP按键即会执行该函数
void button1_callback(const String & state) {
  switchOn = !switchOn;
  digitalWrite(GPIO, switchOn ? HIGH : LOW);
  Blinker.vibrate();
  refreshButton();
}

//小爱电源类操作的回调函数:
//当小爱同学向设备发起控制, 设备端需要有对应控制处理函数
void miotPowerState(const String & state)
{
  BLINKER_LOG("need set power state: ", state);
  if (state == BLINKER_CMD_ON) {
    digitalWrite(GPIO, HIGH);
    switchOn = true;
    BlinkerMIOT.powerState("on");
    BlinkerMIOT.print();
  }
  else if (state == BLINKER_CMD_OFF) {
    digitalWrite(GPIO, LOW);
    switchOn = false;
    BlinkerMIOT.powerState("off");
    BlinkerMIOT.print();
  }
   refreshButton();
}

//接收数据后回调
void dataRead(const String & data)
{
  BLINKER_LOG("Blinker readString: ", data);
  uint32_t BlinkerTime = millis();
  Blinker.print("time:" + BlinkerTime);
  Blinker.print("state:" + switchOn ? "on" : "off");
}

// 更新开关状态
void refreshButton() {
  //Button1.icon("icon_1");
  Button1.color(switchOn ? "#0000ff" : "#aaaaaa");
  Button1.text(switchOn ? "关" : "开");
  Button1.print(switchOn ? "on" : "off");
}

//心跳包
void heartbeat()
{
  bool isOn = digitalRead(GPIO) == HIGH;
  if (isOn) BUILTIN_SWITCH.print("on");
  else  BUILTIN_SWITCH.print("off");
  if (isOn != switchOn) {
    switchOn = isOn;
    refreshButton();
  }
}

void setup() {
  // 初始化串口，并开启调试信息，调试用可以删除
  //Serial.begin(115200);
  //BLINKER_DEBUG.stream(Serial);
  // 初始化IO
  pinMode(GPIO, OUTPUT);
  digitalWrite(GPIO, HIGH);

  // 初始化blinker
  Blinker.begin(auth);
  
  Button1.attach(button1_callback);

  Blinker.attachData(dataRead);//注册数据回调函数
  Blinker.attachHeartbeat(heartbeat);//注册心跳包回调函数
  //小爱同学务必在回调函数中反馈该控制状态
  BlinkerMIOT.attachPowerState(miotPowerState);//注册回调函数

}

void loop() {
  Blinker.run();
}
