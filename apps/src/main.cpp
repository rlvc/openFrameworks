#include "ofMain.h"
#include "ofApp.h"

#include  <iostream>
#include <string>


static void Help();

//========================================================================
int main(int argc, char *argv[]){

	if (argc < 3)
	{
		cout << "invalid command line paramter" << endl;
		Help();
		return 1;
	}

	int ret = 0;

	string inputModel = string(argv[1]);
	string scanOutput = string(argv[2]);

	ofSetupOpenGL(640,480,OF_WINDOW);			// <-------- setup the GL context

	// this kicks off the running of my app
	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:
	ofApp* scanner = new ofApp();
	scanner->setScanInput(inputModel); //"huaping-realsize_22cm_fbx/huaping-realsize_22cm_fbx.FBX")
    if (4 == argc)
    {
        string inputParam = string(argv[3]);
        scanner->setScanParam(inputParam);
    }
	//std::string scanOutput = ofFilePath::getCurrentWorkingDirectory();
	//scanOutput = ofFilePath::join(scanOutput, "scan_out");
	scanner->setScanOutput(scanOutput);
	ret = ofRunApp(scanner);

	return ret;

}



static void Help()
{
	cout << endl << endl
		<< "*********** objScanner Help ***********" << endl << endl
		<< "Usage:" << endl
		<< "    objScanner [model_file] [output_folder]" << endl << endl;

	cout << endl << endl;
	cout << "********** objScanner Help ***********" << endl << endl;
}