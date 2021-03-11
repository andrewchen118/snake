import cc.arduino.*;
import org.firmata.*;

import processing.serial.*;

import processing.serial.*;
import ddf.minim.*;

Serial myPort;
Minim minim;
AudioPlayer player;
byte STARTAUDIO = 97;
byte EATAUDIO = 98;
byte DEATHAUDIO = 99;
 
void setup()
{
  String portName = Serial.list()[0];
  myPort = new Serial(this, portName, 9600);
  minim = new Minim(this);
}
 
void draw() {
  while(myPort.available() > 0){
    int inByte = myPort.read();
    if (inByte == DEATHAUDIO) {
      player = minim.loadFile("deathaudio.wav");
      player.rewind();
      player.play();
    }
    else if (inByte == STARTAUDIO) {
    player = minim.loadFile("startaudio.wav");
      player.rewind();
      player.play();
    }
  }
}

void stop (){
  player.close();
  minim.stop();
}
