#ifndef _PHONE_INPUT_H_
#define _PHONE_INPUT_H_
#include <boost/asio.hpp>
#include <vector>
//#include <winsock2.h>
#include <osg/Image>
#include <osg/GraphicsContext>
#include <osgViewer/Viewer>
#include <osgViewer/CompositeViewer>
#include <string>
#include <queue>

using std::queue;
using std::string;
using boost::asio::ip::tcp;

static queue<string> g_inputMsg;    //������Ϣ
static queue<string> g_outputMsg;   //�����Ϣ

enum MSGType
{
	NONE					=  0,
	NEWCONNECT				=  1,     //������
	DISCONNECT				=  2,     //�Ͽ�����
	GIVEUPCONTROL 			=  3,     //��������
	REQUESTCONTROL			=  4,     //�������
	LOCKNAV					=  5,     //��������
	UNLOCKNAV				=  6,     //�򿪵���
	PUSH					=  7,     //����(��ָ)
	RELEASE					=  8,     //�ſ���ָ(��ָ)
	DRAG					=  9,     //��ָ��ק(��ָ)
	MOVE					= 10,     //phone�������˶�����������ƶ�
	VOLUMEUP				= 11,     //������ť�Ӵ�
	VOLUMEDOWN				= 12,	  //������ť��С
	RAYCASTMANIPULATOR		= 13,     //ȱʡָ��
	TRACKBALLMANIPULATOR	= 14,
	FLIGHTMANIPULATOR		= 15,
	DRIVEMANIPULATOR		= 16,
	UFOMANIPULATOR			= 17,
	NAVMANIPULATOR0			= 18,
	NAVMANIPULATOR1			= 19,
	//��ָ��Ϣ
};
//����׶�
enum TouchPhase 
{
	TOUCH_UNKNOWN,
	TOUCH_BEGAN,
	TOUCH_MOVED,
	TOUCH_STATIONERY,
	TOUCH_ENDED
};
//����ṹ
struct TouchPoint
{
	unsigned int id;          //������
	TouchPhase phase;         //�׶�
	float x, y;               //����
};
//���ٶȼ�
struct Accelerometers
{	
	float _ax, _ay, _az;
};
//������
struct Gyroscope
{
	float gx, gy, gz;
};
//������
struct Magnetometers
{
	float _mx, _my, _mz;
};
//����Ƕ�
struct Rotational
{
	float _roll, _yaw, _pitch;
};

//�ֻ�״̬
typedef struct PHONESTATE
{
	string phoneID;                     //�豸ID
	LONG time;                          //ʱ���
	MSGType msgtype;					//��Ϣ����
	Accelerometers acc;					//���ٶȼ�
	Gyroscope gyr;						//������
	Magnetometers mag;					//������
	Rotational rot;						//����Ƕ�???
	UINT tpnum;							//ͬʱ�������
	TouchPoint touch[5];				//����ṹ�����5��???
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

	//���Ӵ���
	void start();

	
protected:
	PhoneInput();
	virtual ~PhoneInput();
	bool _ready;
	//TCP����
private:
	osgViewer::Viewer *_viewer;
	
	Controller defaultController;	
	std::vector<Controller*> _Controller;
	std::vector<osg::ref_ptr<osgGA::CameraManipulator>> _Manipulators;
	std::map<string,Controller*> IDandControllermaps;


	//boost::asio::io_service io_service;
	//boost::asio::ip::tcp::endpoint ep;
	//boost::asio::ip::tcp::acceptor acceptor;
	//tcp::iostream m_socketStream;
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