#include <string>
#include <fstream>
#include "GPSInterp.h"
#include "LMS511Data.h"
#include "PointCloudBuilder.h"
#include "MergeTime.h"
#include "ConfigReader.h"

#define CATCH 0
#define PROGRESS 1
#define DBUG 0

#if PROGRESS
	int countLidarLines(std::string fineName);
#endif
void Error(ConfigReader *reader);
LMS400Scan* getLidar(int lidarType, LMS400Scan *lidar);

using namespace std;

int main()
{
	string outFileName;
	int timeoffsetMS;
	string line;
	ifstream lidarFile;
	GPSInterp gps;
	ConfigReader *configuration;
	PointCloudBuilder builder;
	LMS400Scan* lidar; // All curent lidar classes are built from this base class
	int lidarType=0;  //  used to set the lidar pointer

	lidar=new(LMS400Scan);
	int currentScan=0;
	int currentStep=0;
	int totalScans;
	bool done=false;

	try
	{
		string ConfigurationFileName;
		cout<<"Enter The Configuration File Name: ";
		getline(cin, ConfigurationFileName);

		configuration = new(ConfigReader);
		configuration->read(ConfigurationFileName);

		#if PROGRESS
		totalScans=countLidarLines(configuration->getLidarFileName());
		#endif

		lidarFile.open(configuration->getLidarFileName());
		lidarType=configuration->getLidarType();
		if(!lidarFile.is_open()){throw "Could NOT open Lidar File.";}

		gps.openFile(configuration->getGPSFileName());
		outFileName=configuration->getOutputFileName();

		cout<<"BEFORE!\n";
		cout<<"configuration\n";
		cout<<"Pitch: "<<configuration->getPitch()<<endl;
		cout<<"Roll: "<<configuration->getRoll()<<endl;

		cout<<"\nGPS\n";
		cout<<"Pitch: "<<gps.getPitchOffset()<<endl;
		cout<<"Roll: "<<gps.getRollOffset()<<endl;

		if(!(gps.getPitchOffset()+.5>configuration->getPitch()&&
			gps.getPitchOffset()-.5<configuration->getPitch()))
		{
			configuration->setPitch(configuration->getPitch()+gps.getPitchOffset());
		}
		
		if(!(gps.getRollOffset()+.5>configuration->getRoll()&&
			gps.getRollOffset()-.5<configuration->getRoll()))
		{
			configuration->setRoll(configuration->getRoll()+gps.getRollOffset());
		}

		cout<<"\n\nAfter!\n";
		cout<<"Pitch: "<<configuration->getPitch()<<endl;
		cout<<"Roll: "<<configuration->getRoll()<<endl;

		gps.setOffsetDist(configuration->getMountingXYDist());
		gps.setOffsetAngle(configuration->getMountingAngle());

		builder.setMountingHeight(configuration->getMountingHeight()); 

		builder.setBounds(configuration->getLowerBounds(),configuration->getUpperBounds());
		builder.setShift(configuration->getShift());
		builder.setMaxPoints(configuration->getExcerptNum());
		builder.setLeadingPoints(configuration->getLeadingPoints());
		//builder.doNormals();

	}
	catch(const char *e)
	{
		cout<<e<<endl;
		Error(configuration);
	}
	catch(...)
	{
		cout<<"Not sure what went Wrong\n";
		Error(configuration);
	}

	while(getline(lidarFile,line) && !done)
	{
		try
		{
		#if PROGRESS
		currentScan ++;
		if(currentScan>currentStep)
		{
			std::cout<<"\tProgress: "<<currentScan/1000
			<<" of "<<totalScans/1000<<endl;
			currentStep += 1000;
		}
		#endif

		if(line.find("RECEIVE")==string::npos){throw "Lidar record corrupted.\n";}

		MergeTime transmitionTime;
		transmitionTime.setStamp(line);
		#if DBUG
				cout <<"Found time stamp"<<endl; 
		#endif
	
		ExtendedGps location;
		location = gps.getLocation(transmitionTime.getTime());
		#if DBUG
				cout <<"Updated/recived GPS"<<endl; 
		#endif
		lidar=getLidar(lidarType,lidar);
		lidar->setScan(line);
		lidar->decode();
		#if DBUG
				cout <<"Decoded Lidar"<<endl; 
		#endif
		builder.setRoll(configuration->getRoll()+location.getRoll());
		builder.setPitch(configuration->getPitch()+location.getPitch());
		builder.setYaw(configuration->getYaw());

		builder.addPoints(lidar->getDistValues(),lidar->getStartAngle(), lidar->getAngularStep(), lidar->getScaler());
		builder.rotateRow(location.getYaw());
		builder.placeRow(location.getNorthing(), location.getEasting());
		done=builder.isDone();
		#if DBUG
				cout <<"Processed a line"<<endl<<endl; 
		#endif
		
		}
		catch(const char * e)
		{
			#if CATCH
				cout <<e<<endl<<endl;
			#endif
		}
		catch(const std::exception &exc)
		{
			#if CATCH
				cout<<exc.what();
			#endif
		}
		catch(...)
		{
			#if CATCH
				cout <<"Unexpected Error."<<endl<<endl;
			#endif
		}
	}

	builder.writeFile(outFileName);

	return 0;
}


#if PROGRESS
int countLidarLines(std::string fileName)
{
	int count =0;
	std::ifstream lidar;
	std::string line;
	for(lidar.open(fileName); getline(lidar,line); count++){}
	lidar.close();
	return count;
}
#endif


void Error(ConfigReader *reader)
{
	string answer;
	cout<<"\tWould you like a sample configuration file to print? (y/n)";
	getline(cin,answer);
	if(answer.find("y") != string::npos ||answer.find("Y") != string::npos)
	{
		cout<<endl<<endl<<reader->makeEmptyConfigFile()<<endl<<endl;
	}
	delete(reader);


	exit(EXIT_FAILURE);
}


LMS400Scan* getLidar(int lidarType, LMS400Scan *lidar)
{
	if(lidar!=NULL)
	{
		delete(lidar);
	}

	switch(lidarType)
	{
		case 0:
			return new(LMS511Scan);
		case 1:
		break;
	}
	return new(LMS511Scan);
}