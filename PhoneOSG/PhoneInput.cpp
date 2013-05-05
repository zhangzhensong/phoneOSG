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
		//�Ӿ���ȱʡ����Ͳ�����Ϊ��һ��,
		defaultController._Cameras = _viewer->getCamera();
		defaultController._Manipulator = _viewer->getCameraManipulator();

		osg::Image* image = new osg::Image;
		osg::Viewport* a = defaultController._Cameras->getViewport();
		image->allocateImage(a->width(),a->height() , 1, GL_RGBA, GL_UNSIGNED_BYTE);
		defaultController._Images = image;
		_Manipulators.push_back(defaultController._Manipulator);
		IDandControllermaps["zfj"] = &defaultController;
		//һ�μ������в�����
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
	/*��ȡ
	NEWCONNECT				=  1,     //������
	DISCONNECT				=  2,     //�Ͽ�����
	GIVEUPCONTROL 			=  3,     //��������
	REQUESTCONTROL			=  4,     //�������
	��������Ϣ,���Ӿ������\�Ƴ����\�л��������*/
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
	//�˴���Ҫ�����ͳһ����Ϣ��ʽ��Ȼ�����ж���Ϣ���ͣ�����ͬ�����ͽ���
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
	//��ȡ��Ϣ����

	osg::ref_ptr<PhoneEvent> event = new PhoneEvent;
	//��װ�¼�
	{
		boost::unique_lock<boost::mutex> lock(g_mutex); 
		while(!m_inputMsg.empty())
		{
			ptree msg;

			msg = m_inputMsg.front();
			m_inputMsg.pop();

			msg2event(msg, event.get());  //��stringת��event

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
		tcp::iostream s("10.0.0.90", "7777");  //��server�˵�ip��port
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

					set<int>::iterator it;       //��������phoneID

					switch(msgType)
					{
					case NEWCONNECT:
					case DISCONNECT:
					case GIVEUPCONTROL:
					case REQUESTCONTROL:
						m_controlMsg.push(pt);
						break;
					default:
						it = m_nsetPhoneIDs.find(phoneID);   //����phone��û������Ļ�����Щ������Ϣ���Թ��˵�
						if(it != m_nsetPhoneIDs.end())
							m_inputMsg.push(pt);
					}

					line.clear();
				}
			}

			//std::cin >> line;          //ע��������շ��߼����Է������˷�һ��ָ��

			//ptree pt;
			//istringstream istream(line);
			//std::cout << line << std::endl;
			//read_json<ptree>(istream, pt);

			//s << line << std::endl;    //����һ���ӷ������˷��ص���Ϣ��ע��˴���������Base64��װ��ͼ�񣬻�������Json��װ�����л��ַ���
			//s.flush();               //ˢ�������壬�ѻ�������������ͳ�ȥ��ʵ���ϲ��Ե�ʱ�����й�ϵ����
		}
		s.clear();
		s.close();
	}
	catch (std::exception& e)
	{
		std::cout << "Exception: " << e.what() << std::endl;
	}
}