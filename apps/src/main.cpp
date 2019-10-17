#include "ofMain.h"
#include "ofApp.h"

#include  <iostream>
#include <string>


static void Help();

//========================================================================
int main(int argc, char *argv[]){

	if (argc < 4)
	{
		cout << "invalid command line paramter" << endl;
		Help();
		return 1;
	}

	int ret = 0;

	ofSetupOpenGL(640,480, OF_WINDOW_EGL_OS);			// <-------- setup the GL context

	// this kicks off the running of my app
	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:
	ofApp* scanner = new ofApp();

	string inputModel = string(argv[1]);
	scanner->setScanInput(inputModel);
	string scanOutput = string(argv[2]);
	scanner->setScanOutput(scanOutput);
	string extrinsics_param = string(argv[3]);
	scanner->setExtrinsicsParam(extrinsics_param);
	if (argc >= 5)
	{
		string inputParam = string(argv[4]);
		scanner->setScanParam(inputParam);
	}
    	
	ret = ofRunApp(scanner);

	return ret;

}



static void Help()
{
	cout << endl << endl
		<< "*********** fealessScanner Help ***********" << endl << endl
		<< "Usage:" << endl
		<< "    fealessScanner [model_file] [output_folder] [extrinsics_param] [scan_param]" << endl << endl;

	cout << endl << endl;
	cout << "********** fealessScanner Help ***********" << endl << endl;
}