// PhoneOSG.cpp : 定义控制台应用程序的入口点。
#include "stdafx.h"
#include <osgViewer/CompositeViewer>
#include <osgViewer/Viewer>
#include <osgViewer/api/Win32/GraphicsWindowWin32>
#include <osgDB/ReadFile>
#include <osgGA/TrackballManipulator>
#include <iostream>
#include <boost/thread/thread.hpp>
#include "PhoneInput.h"

using namespace std;

class MDEViewer : public osgViewer::Viewer
{
public:
	MDEViewer() : osgViewer::Viewer() {}
	virtual ~MDEViewer() {}

	virtual void eventTraversal()
	{
		PhoneInput::instance()->updateState( _eventQueue.get() );
		//cout << "eventTraversal\n"<<endl;
		osgViewer::Viewer::eventTraversal();
	}

protected:
	virtual void viewerInit()
	{
		osgViewer::GraphicsWindowWin32* windowWin32 =
			dynamic_cast<osgViewer::GraphicsWindowWin32*>( _camera->getGraphicsContext() );
		if ( windowWin32 )
		{
			HWND hwnd = windowWin32->getHWND();
			//PhoneInput::instance()->initPhone(hwnd);
			////DirectInputRegistry::instance()->initMouse( hwnd );
			//DirectInputRegistry::instance()->initJoystick( hwnd );
		}
		osgViewer::Viewer::viewerInit();
	}
};

class PhoneHandler : public osgGA::GUIEventHandler
{
public:
	PhoneHandler() {}

	bool handle( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa )
	{
		switch ( ea.getEventType() )
		{
		case osgGA::GUIEventAdapter::KEYDOWN:
			std::cout << "*** Key 0x" << std::hex << ea.getKey() << std::dec << " down ***" << std::endl;
			break;
		case osgGA::GUIEventAdapter::KEYUP:
			std::cout << "*** Key 0x" << std::hex << ea.getKey() << std::dec << " up ***" << std::endl;
			break;
		case osgGA::GUIEventAdapter::USER:
			{
				const PhoneEvent* phoneEvent = dynamic_cast<const PhoneEvent*>( ea.getUserData() );
				if ( !phoneEvent ) break;

				switch(phoneEvent->_ps.msgtype )
				{
				case NONE:
					cout << "NONE" << endl;
					//std::cout << pt.get<string>("x") << std::endl;
					//std::cout << pt.get<string>("y") << std::endl;
					//phoneEvent->
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
					cout << phoneEvent->_ps.rot._roll << endl;
					cout << phoneEvent->_ps.rot._yaw << endl;
					cout << phoneEvent->_ps.rot._pitch << endl;
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
			return true;
		default:
			cout << ea.getEventType() << endl;
			cout << osgGA::GUIEventAdapter::USER << endl;
			break;
		}
		return false;
	}
};


int _tmain(int argc, char** argv)
{
	osg::ArgumentParser arguments( &argc, argv );
	osg::Node* model = osgDB::readNodeFiles( arguments );
	if ( !model ) model = osgDB::readNodeFile( "cow.osg" );
	if ( !model ) 
	{
		std::cout << arguments.getApplicationName() <<": No data loaded" << std::endl;
		return 1;
	}

	boost::thread thrd(boost::bind(&PhoneInput::start, (PhoneInput::instance())));
	thrd.detach();  
	//需要这一步

	MDEViewer viewer;
	viewer.setUpViewInWindow( 250, 50, 800, 600 );
	viewer.setSceneData( model );
	//viewer.addEventHandler( new PhoneHandler );
	viewer.setCameraManipulator(new osgGA::TrackballManipulator());
	PhoneInput::instance()->init(viewer);
	//viewer.addEventHandler( new JoystickHandler );
	//viewer.addEventHandler( new osgViewer::StatsHandler );
	//viewer.addEventHandler( new osgViewer::WindowSizeHandler );
	//viewer.addEventHandler( new osgGA::StateSetManipulator(viewer.getCamera()->getOrCreateStateSet()) );


	viewer.realize();
	viewer.frame();
//	return viewer.run();
	while (!viewer.done())
	{	
		PhoneInput::instance()->updateViewer(viewer);
		viewer.advance();
		//updateUserStats(viewer);		
		viewer.eventTraversal();
		viewer.updateTraversal();
		viewer.renderingTraversals();
	}
	return 0;
}

