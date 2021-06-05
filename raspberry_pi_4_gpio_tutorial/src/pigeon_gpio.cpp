#include <ros/ros.h>
#include <wiringPi.h>
#include <softPwm.h>
#include <geometry_msgs/Twist.h>
#include <termios.h>

#define LED_PIN 12

double x;

void CmdVelCallback(const geometry_msgs::Twist &cmd_vel)
{
  x = cmd_vel.linear.x;
}


int input_return_key()
{
  struct termios org_term;

  char input_key = 0;

  tcgetattr(STDIN_FILENO, &org_term);

  struct termios new_term = org_term;

  new_term.c_lflag &= ~(ECHO | ICANON);

  new_term.c_cc[VMIN] = 0;
  new_term.c_cc[VTIME] = 0;

  tcsetattr(STDIN_FILENO, TCSANOW, &new_term);

  read(STDIN_FILENO, &input_key, 1);

  tcsetattr(STDIN_FILENO, TCSANOW, &org_term);

  return input_key;
}

int main(int argc, char **argv)
{
  ros::init(argc, argv, "pigeon_gpio_node");
  ros::NodeHandle n;

  ros::Subscriber subscriber_cmd_vel;
  subscriber_cmd_vel = n.subscribe("cmd_vel", 1000, CmdVelCallback);


  int a = 0;

  ros::Rate loop_rate(1);

  wiringPiSetupGpio();
  pinMode(20, OUTPUT);
  pinMode(21, OUTPUT);
  pinMode(12, OUTPUT);
  softPwmCreate(12, 0, 100);
  
  while(ros::ok())
  {  
     softPwmWrite(12,int(abs(x*100)));

     if(x>0)
     {
     digitalWrite(20,HIGH);
     digitalWrite(21,LOW);
     }
     else if (x <0)
     {
     digitalWrite(20,LOW);
     digitalWrite(21,HIGH);
     }
     else if (x ==0)
     {
     digitalWrite(20,LOW);
     digitalWrite(21,LOW);
     }
 
    ros::spinOnce();
    loop_rate.sleep();
  }
   digitalWrite(12,LOW);
   return 0;
}
