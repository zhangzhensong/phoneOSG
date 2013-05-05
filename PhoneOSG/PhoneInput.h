#ifndef _PHONE_INPUT_H_
#define _PHONE_INPUT_H_
#include <boost/asio.hpp>
#include <vector>
//#include <winsock2.h>
#include <osg/Image>
#include <osg/GraphicsContext>
#include <osgViewer/Viewer>
#include <osgViewer/CompositeViewer>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/date_time.hpp>
#include <string>
#include <queue>
#include <set>

using std::set;
using std::queue;
using std::string;
using boost::asio::ip::tcp;
using namespace boost::property_tree;

enum MSGType
{
	NONE					=  0,
	NEWCONNECT				=  1,     //新连接
	DISCONNECT				=  2,     //断开连接
	GIVEUPCONTROL 			=  3,     //放弃控制
	REQUESTCONTROL			=  4,     //请求控制
	LOCKNAV					=  5,     //锁定导航
	UNLOCKNAV				=  6,     //打开导航
	PUSH					=  7,     //单击(单指)
	RELEASE					=  8,     //放开手指(单指)
	DRAG					=  9,     //手指拖拽(单指)
	MOVE					= 10,     //phone的自由运动，类似鼠标移动
	VOLUMEUP				= 11,     //音量按钮加大
	VOLUMEDOWN				= 12,	  //音量按钮减小
	RAYCASTMANIPULATOR		= 13,     //缺省指点
	TRACKBALLMANIPULATOR	= 14,
	FLIGHTMANIPULATOR		= 15,
	DRIVEMANIPULATOR		= 16,
	UFOMANIPULATOR			= 17,
	NAVMANIPULATOR0			= 18,
	NAVMANIPULATOR1			= 19,
	//多指消息
};

//触点阶段
enum TouchPhase 
{
	TOUCH_UNKNOWN,
	TOUCH_BEGAN,
	TOUCH_MOVED,
	TOUCH_STATIONERY,
	TOUCH_ENDED
};

//触点结构
struct TouchPoint
{
	unsigned int id;          //触点编号
	TouchPhase phase;         //阶段
	float x, y;               //坐标
};

//加速度计
struct Accelerometers
{	
	float _ax, _ay, _az;
};

//陀螺仪
struct Gyroscope
{
	float gx, gy, gz;
};

//磁力计
struct Magnetometers
{
	float _mx, _my, _mz;
};

//虚拟角度
struct Rotational
{
	float _roll, _yaw, _pitch;
};

//手机状态
typedef struct PHONESTATE
{
	string phoneID;                     //设备ID
	LONG time;                          //时间戳
	MSGType msgtype;					//消息类型
	Accelerometers acc;					//加速度计
	Gyroscope gyr;						//陀螺仪
	Magnetometers mag;					//磁力计
	Rotational rot;						//虚拟角度???
	UINT tpnum;							//同时触点个数
	TouchPoint touch[5];				//触点结构，最多5点???
} PhoneState;

class PhoneEvent : public osg::Referenced
{
public:
	PhoneEvent() {}
	virtual ~PhoneEvent() {}
	PHONESTATE _ps;
};

struct Controller
{
	osg::ref_ptr<osg::Camera> _Cameras;
	osg::ref_ptr<osgGA::CameraManipulator> _Manipulator;
	osg::ref_ptr<osg::Image>  _Images;
};

class PhoneInput : public osg::Referenced
{
public:
	static PhoneInput* instance()
	{
		static osg::ref_ptr<PhoneInput> s_registry = new PhoneInput;
		return s_registry.get();
	}

	bool init( osgViewer::Viewer &viewer );
	void updateViewer(osgViewer::Viewer &viewer);
	void updateState( osgGA::EventQueue* eventQueue );

	//连接处理
	void start();

	
protected:
	PhoneInput();
	virtual ~PhoneInput();
	bool _ready;
	//TCP连接
private:
	osgViewer::Viewer *_viewer;
	
	Controller defaultController;	
	std::vector<Controller*> _Controller;
	std::vector<osg::ref_ptr<osgGA::CameraManipulator>> _Manipulators;
	std::map<string,Controller*> IDandControllermaps;

	queue<ptree> m_controlMsg;  //只包含断开连接，新连接，请求控制和放弃控制等
	queue<ptree> m_inputMsg;    //输入信息,不包括连接，申请放弃控制权的消息
	queue<ptree> m_outputMsg;   //输出信息

	int m_nMainPhoneID;         //主相机ID,如果没人控制，那么初始化为0，如果要申请控制，则需要检查该标志
	
	//所有的相机ID，编号从0开始的整数，如果需要有新的连接，判断是否已经有连接
	//如果需要删除相机，也需要判断是否还存在该相机
	//在插入操作消息之前，需要先检查该手机是否连接上
	set<int> m_nsetPhoneIDs;
};

#endif

/*
class JoystickEvent : public osg::Referenced
{
public:
	JoystickEvent() {}
	virtual ~JoystickEvent() {}
	DIJOYSTATE2 _js;
};

class DirectInputRegistry : public osg::Referenced
{
public:
	static DirectInputRegistry* instance()
	{
		static osg::ref_ptr<DirectInputRegistry> s_registry = new DirectInputRegistry;
		return s_registry.get();
	}

	LPDIRECTINPUT8& getDevice() { return _inputDevice; }
	LPDIRECTINPUTDEVICE8& getKeyboard() { return _keyboard; }
	LPDIRECTINPUTDEVICE8& getMouse() { return _mouse; }
	LPDIRECTINPUTDEVICE8& getJoyStick() { return _joystick; }

	bool valid() const { return _supportDirectInput; }

	bool initKeyboard( HWND handle );
	bool initMouse( HWND handle );
	bool initJoystick( HWND handle );

	void updateState( osgGA::EventQueue* eventQueue );

protected:
	DirectInputRegistry();
	virtual ~DirectInputRegistry();

	bool initImplementation( HWND handle, LPDIRECTINPUTDEVICE8 device, LPCDIDATAFORMAT format );
	void pollDevice( LPDIRECTINPUTDEVICE8 device );
	void releaseDevice( LPDIRECTINPUTDEVICE8 device );

	static BOOL CALLBACK EnumJoysticksCallback( const DIDEVICEINSTANCE* didInstance, VOID* );

	LPDIRECTINPUT8 _inputDevice;
	LPDIRECTINPUTDEVICE8 _keyboard;
	LPDIRECTINPUTDEVICE8 _mouse;
	LPDIRECTINPUTDEVICE8 _joystick;
	bool _supportDirectInput;
};

#endif
*/