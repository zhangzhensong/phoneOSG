#include "stdafx.h"
#include <boost/thread/condition_variable.hpp> 

#include "PhoneInput.h"
#include "Base64.h"

using namespace boost::property_tree;
using namespace boost::gregorian;
using namespace boost;
using namespace std;
using std::string;
using boost::asio::ip::tcp;

boost::mutex g_mutex;


int zfj =0;
std::string make_daytime_string()
{
	using namespace std; // For time_t, time and ctime;
	time_t now = time(0);
	return ctime(&now);
}

using boost::asio::ip::tcp;

PhoneInput::PhoneInput() :_ready(false)
{
}

PhoneInput::~PhoneInput()
{	
}

bool PhoneInput::init(osgViewer::Viewer &viewer )
{
	if(!_ready)
	{
		_viewer = &viewer;
		//视景器缺省相机和操作器为第一个,
		defaultController._Cameras = _viewer->getCamera();
		defaultController._Manipulator = _viewer->getCameraManipulator();

		osg::Image* image = new osg::Image;
		osg::Viewport* a = defaultController._Cameras->getViewport();
		image->allocateImage(a->width(),a->height() , 1, GL_RGBA, GL_UNSIGNED_BYTE);
		defaultController._Images = image;
		_Manipulators.push_back(defaultController._Manipulator);
		IDandControllermaps["zfj"] = &defaultController;
		//一次加入所有操作器
		//_Manipulators.push_back(new osgGA::CameraManipulator());
		//_Manipulators.push_back(new osgGA::CameraManipulator());
		//_Manipulators.push_back(new osgGA::CameraManipulator());
		//_Manipulators.push_back(new osgGA::CameraManipulator());
		//_Manipulators.push_back(new osgGA::CameraManipulator());
		//_Manipulators.push_back(new osgGA::CameraManipulator());
		//_Manipulators.push_back(new osgGA::CameraManipulator());
		_ready = true;
	}	
	return _ready;
}

void PhoneInput::updateViewer(osgViewer::Viewer &viewer)
{
	//cout << "updateViewer \n" << zfj <<endl;
	//zfj++;
	/*获取
	NEWCONNECT				=  1,     //新连接
	DISCONNECT				=  2,     //断开连接
	GIVEUPCONTROL 			=  3,     //放弃控制
	REQUESTCONTROL			=  4,     //请求控制
	这四类消息,对视景器添加\移除相机\切换主相机等*/
	{
		boost::unique_lock<boost::mutex> lock(g_mutex);
		while(!m_controlMsg.empty())
		{
			ptree pt;

			pt = m_controlMsg.front();
			m_controlMsg.pop();

			int msgType = atoi(pt.get<string>("msgType").c_str());
			switch(msgType)
			{
			case NEWCONNECT:
				cout << "NEWCONNECT" << endl;
				break;
			case DISCONNECT:
				cout << "DISCONNECT" << endl;
				break;
			case GIVEUPCONTROL:
				cout << "GIVEUPCONTROL" << endl;
				break;
			case REQUESTCONTROL:
				cout << "REQUESTCONTROL" << endl;
				break;
			default:
				cout << "user Event\n" << endl;
				break;
			}
		}

	}

}

void msg2event(ptree& pt, PhoneEvent* phoneEvent)
{
	ptree pt1, pt2;
	double roll = 0, yaw = 0, pitch = 0; 

	int msgType = atoi(pt.get<string>("msgType").c_str());
	//此处需要定义好统一的消息格式，然后再判断消息类型，按不同的类型解析
	switch(msgType)
	{
	case NONE:
		cout << "NONE" << endl;
		break;
	case NEWCONNECT:
		cout << "NEWCONNECT" << endl;
		break;
	case DISCONNECT:
		cout << "DISCONNECT" << endl;
		break;
	case GIVEUPCONTROL:
		cout << "GIVEUPCONTROL" << endl;
		break;
	case REQUESTCONTROL:
		cout << "REQUESTCONTROL" << endl;
		break;
	case LOCKNAV:
		cout << "LOCKNAV" << endl;
		break;
	case UNLOCKNAV:
		cout << "UNLOCKNAV" << endl;
		break;
	case PUSH:
		cout << "PUSH" << endl;
		break;
	case RELEASE:
		cout << "RELEASE" << endl;
		break;
	case DRAG:
		cout << "DRAG" << endl;
		break;
	case MOVE:
		cout << "MOVE" << endl;
		break;
	case VOLUMEUP:
		cout << "VOLUMEUP" << endl;
		break;
	case VOLUMEDOWN:
		cout << "VOLUMEDOWN" << endl;
		break;
	case RAYCASTMANIPULATOR:
		cout << "RAYCASTMANIPULATOR" << endl;
		break;
	case TRACKBALLMANIPULATOR:
		cout << "TRACKBALLMANIPULATOR" << endl;
		break;
	case FLIGHTMANIPULATOR:
		cout << "FLIGHTMANIPULATOR" << endl;

		pt1 = pt.get_child("Rotational");
		roll = atof(pt1.get<string>("roll").c_str());
		yaw = atof(pt1.get<string>("yaw").c_str());
		pitch = atof(pt1.get<string>("pitch").c_str());

		phoneEvent->_ps.msgtype = FLIGHTMANIPULATOR;

		phoneEvent->_ps.rot._roll = roll;
		phoneEvent->_ps.rot._yaw = yaw;
		phoneEvent->_ps.rot._pitch = pitch;

		break;
	case DRIVEMANIPULATOR:
		cout << "DRIVEMANIPULATOR" << endl;
		break;
	case UFOMANIPULATOR:
		cout << "UFOMANIPULATOR" << endl;
		break;
	case NAVMANIPULATOR0:
		cout << "NAVMANIPULATOR0" << endl;
		break;
	case NAVMANIPULATOR1:
		cout << "NAVMANIPULATOR1" << endl;
		break;
	default:
		cout << "use default" << endl;
		break;
	}

}

void PhoneInput::updateState( osgGA::EventQueue* eventQueue )
{
	//return;
	if (!_ready || !eventQueue)
		return;
	//cout << "updateState \n" << zfj << endl;
	//读取消息队列

	osg::ref_ptr<PhoneEvent> event = new PhoneEvent;
	//组装事件
	{
		boost::unique_lock<boost::mutex> lock(g_mutex); 
		while(!m_inputMsg.empty())
		{
			ptree msg;

			msg = m_inputMsg.front();
			m_inputMsg.pop();

			msg2event(msg, event.get());  //把string转成event

			//cout << event->_ps.rot._roll << endl;
			//cout << event->_ps.rot._pitch << endl;
			//cout << event->_ps.rot._yaw << endl;

			eventQueue->userEvent( event.get() );
		}
	}

	//if ( SUCCEEDED(hr) )

}

void PhoneInput::start()
{
	try
	{
		tcp::iostream s("10.0.0.90", "7777");  //绑定server端的ip和port
		if (!s)
		{
			std::cout << "Unable to connect: " << s.error().message() << std::endl;
			return;
		}

		std::string line;

		while(true)
		{
			std::getline(s, line);
			{
				boost::unique_lock<boost::mutex> lock(g_mutex); 
				if(line.size())
				{
					std::cout << line << std::endl;
					ptree pt;
					istringstream istream(line);
					read_json<ptree>(istream, pt);

					int msgType = atoi(pt.get<string>("msgType").c_str());
					int phoneID = atoi(pt.get<string>("phoneID").c_str());

					set<int>::iterator it;       //用来查找phoneID

					switch(msgType)
					{
					case NEWCONNECT:
					case DISCONNECT:
					case GIVEUPCONTROL:
					case REQUESTCONTROL:
						m_controlMsg.push(pt);
						break;
					default:
						it = m_nsetPhoneIDs.find(phoneID);   //假如phone还没有连入的话，那些控制消息可以过滤掉
						if(it != m_nsetPhoneIDs.end())
							m_inputMsg.push(pt);
					}

					line.clear();
				}
			}

			//std::cin >> line;          //注意这里的收发逻辑，对服务器端发一个指令

			//ptree pt;
			//istringstream istream(line);
			//std::cout << line << std::endl;
			//read_json<ptree>(istream, pt);

			//s << line << std::endl;    //接收一个从服务器端返回的消息，注意此处可以是由Base64包装的图像，或者是由Json包装的序列化字符串
			//s.flush();               //刷新流缓冲，把缓存里的数据推送出去，实际上测试的时候，这行关系不大
		}
		s.clear();
		s.close();
	}
	catch (std::exception& e)
	{
		std::cout << "Exception: " << e.what() << std::endl;
	}
}