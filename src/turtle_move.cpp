// ROSに関する基本的なAPIをインクルード
#include <ros/ros.h>
// geometry_msgsのTwist.msgから生成されたメッセージを定義しているヘッダをインクルード
#include <geometry_msgs/Twist.h>
// turtlesimのPose.msgから生成されたメッセージを定義しているヘッダをインクルード
#include <turtlesim/Pose.h>


// パブリッシャーの定義    (メッセージ送信)
ros::Publisher velocity_publisher;
// サブスクライバーの定義  (メッセージ受信)
ros::Subscriber pose_subscriber;

// vel_msgをgeometry_msgsのTwist型メッセージとして定義
geometry_msgs::Twist vel_msg;
// turtlesim_poseをturtlesimのPose型メッセージとして定義
turtlesim::Pose turtlesim_pose;

// 定数PIを定義
const double PI = 3.14159;

// 関数のプロトタイプ宣言
void poseCallback(const turtlesim::Pose::ConstPtr & pose_message);
void move(double distance);
void rotate(double relative_angle);
void writeCircle(double radius, double turn_angle);


int main(int argc, char **argv)
{
    // ノード名をturtle_moveとしてROSの初期化をする
    ros::init(argc, argv, "turtle_move");
    // 名前nでノードのネームスペースを生成
    ros::NodeHandle n;

    // velocity_publisherは"/turtle1/cmd_vel"というトピック名でTwist型のメッセージを送信するものと定義. 
    // キューのサイズは10.　10個のメッセージを保持する
    velocity_publisher = n.advertise<geometry_msgs::Twist>("/icart_mini/cmd_vel", 10);

    // pose_subscriberは"/turtle1/pose"というトピック名のメッセージを受信するものと定義 
    // /turtle1/poseトピックが更新されるたびposeCallback関数を実行する
    // キューのサイズは10.　10個のメッセージを保持する
    pose_subscriber = n.subscribe("/turtle1/pose", 10, poseCallback);

    // スクリーンに文字を出力
    ROS_INFO("\n************** Start the turtle moving **************\n");

    while(1){
        writeCircle(1.0, PI);
    }
        rotate(PI+PI/2);
        writeCircle(1.0, PI);
        move(1.9);
        rotate(PI);
    
    rotate(PI+PI/2);
    move(4);


    //for(int i = 0; i < 5; i++){
    //    move(1.9);
    //    rotate(PI - 1.884954);
    //}

    ROS_INFO("\n************** Finished the turtle moving **************\n");
    return 0;
}

// velocity_publisher で受信したメッセージが引数pose_messageに与えられる
// 引数pose_messageはturtlesimのPose型メッセージで定義.  受信したメッセージを参照先とする定数
// 受信したメッセージをグローバル定義したturtlesim_pose へ代入
void poseCallback(const turtlesim::Pose::ConstPtr & pose_message)
{
    turtlesim_pose.x = pose_message -> x;
    turtlesim_pose.y = pose_message -> y;
    turtlesim_pose.theta = pose_message -> theta;
}

// 亀のx,y座標を元に進んだ距離を計算
// 進んだ距離が引数distanceより大きくなるまで亀を動かす
void move(double distance)
{

    double pose0_x = 0, pose0_y = 0;
    double pose1_x = 0, pose1_y = 0;
    double current_distance = 0;
    // loop_rate.sleep() を実行すると1/1000秒待つ
    ros::Rate loop_rate(1000);

    // poseCallbackによってturtlesim_poseの値が更新されるまでループする
    do{
        // 1回だけサブスクライバーを実行する= pose_subscriberが呼ばれメッセージを受信し、poseCallbackを呼ぶ
        ros::spinOnce();
        pose0_x = turtlesim_pose.x; 
        pose0_y = turtlesim_pose.y;
    }while(pose0_x == 0 || pose0_y == 0);

    vel_msg.linear.x = 2;
    do{
        // メッセージ(vel_msg)をパブリッシュ
        velocity_publisher.publish(vel_msg);
        // 1回だけサブスクライバーを実行する= pose_subscriberが呼ばれメッセージを受信し、poseCallbackを呼ぶ
        ros::spinOnce();
        pose1_x = turtlesim_pose.x;
        pose1_y = turtlesim_pose.y;

        if((pose1_x - pose0_x)==0)
            current_distance = pose1_y - pose0_y;
        else if((pose1_y - pose0_y)==0)
            current_distance = pose1_x - pose0_x;
        else
            current_distance = sqrt(pow(pose1_x - pose0_x, 2) + pow(pose1_y - pose0_y, 2));

        // 1/1000秒待つ
        loop_rate.sleep();
    }while(fabs(current_distance) < distance);

    vel_msg.linear.x = 0;
    // メッセージ(vel_msg)をパブリッシュ
    velocity_publisher.publish(vel_msg);
}


// 亀のthetaを元に回転した角度を計算
// 回転した角度が引数target_angleより大きくなるまで亀を回転させる→その場回転させる
void rotate(double target_angle)
{
    // loop_rate.sleep() を実行すると1/1000秒待つ
    ros::Rate loop_rate(1000);
    double current_angle = 0;

    vel_msg.angular.z = 1.0;
    target_angle += turtlesim_pose.theta;
    if(target_angle > 2*PI) 
        target_angle = fmod(target_angle, 2*PI);

    do{
        // メッセージ(vel_msg)をパブリッシュ
        velocity_publisher.publish(vel_msg);
        current_angle = turtlesim_pose.theta;
        // 1回だけサブスクライバーを実行する= pose_subscriberが呼ばれメッセージを受信し、poseCallbackを呼ぶ
        ros::spinOnce();
        // 1/1000秒待つ
        loop_rate.sleep();
    }while(fabs(target_angle - current_angle) > 0.01);

    vel_msg.angular.z =0;
    // メッセージ(vel_msg)をパブリッシュ
    velocity_publisher.publish(vel_msg);
}


// 半径(radius)の円をtarget_angle ラジアン描く
void writeCircle(double radius, double target_angle)
{
    // loop_rate.sleep() を実行すると1/1000秒待つ
    ros::Rate loop_rate(1000);
    double current_angle = 0.0;

    vel_msg.angular.z = 1.0;
    vel_msg.linear.x = fabs(radius * vel_msg.angular.z);

    target_angle += turtlesim_pose.theta;
    if(target_angle > 2*PI) 
        target_angle = fmod(target_angle, 2*PI);

    do{
        // メッセージ(vel_msg)をパブリッシュ
        velocity_publisher.publish(vel_msg);
        current_angle = turtlesim_pose.theta;
        // 1回だけサブスクライバーを実行する= pose_subscriberが呼ばれメッセージを受信し、poseCallbackを呼ぶ
        ros::spinOnce();
        // 1/1000秒待つ
        loop_rate.sleep();
    }while(fabs(target_angle - current_angle) > 0.01);

    vel_msg.angular.z = 0;
    vel_msg.linear.x = 0;
    // メッセージ(vel_msg)をパブリッシュ
    velocity_publisher.publish(vel_msg);
}


