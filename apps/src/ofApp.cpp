#include "ofApp.h"
#include "ofxAssimpModelLoader.h"

GLfloat lightOnePosition[] = { 40.0, 40, 100.0, 0.0 };
GLfloat lightOneColor[] = { 0.99, 0.99, 0.99, 1.0 };

GLfloat lightTwoPosition[] = { -40.0, 40, 100.0, 0.0 };
GLfloat lightTwoColor[] = { 0.99, 0.99, 0.99, 1.0 };

ofApp::ofApp()
{
	_objModel = nullptr;
	_mainCamera = nullptr;
	_frameCount = 0;

    _camera_rotation_step = 10.0f;
    _vertical_rotation_step = 15.0f;
    _vertical_rotation_max = 105.0f;
    _scale_step = 0.1f;
    _scale_max = 0.6f;
    _model_scale = 1.0f;
}

ofApp::~ofApp()
{
	if (_objModel) delete _objModel;
	_objModel = nullptr;

	if (_mainCamera) delete _mainCamera;
	_mainCamera = nullptr;
}

//--------------------------------------------------------------
void ofApp::setup(){

	if (!_objModel) {
		_objModel = new ofxAssimpModelLoader();
	}

	if (!_mainCamera) {
		_mainCamera = new ofCamera();
	}

	_objModel->setScaleNormalization(false);
	
	if (!_objModel->loadModel(_inputModel, 20)) {
		ofExit(2);
	}

	ofFbo::Settings fboSettings;
	fboSettings.width = ofGetWidth();
	fboSettings.height = ofGetHeight();
	fboSettings.internalformat = GL_RGBA;
	fboSettings.numSamples = 0;
	fboSettings.depthStencilAsTexture = false;

	//_fbo_GL_RGBA.allocate(fboSettings);
	_fbo_GL_RGBA.allocate(ofGetWidth(), ofGetHeight(), GL_RGBA, 0);

	ofEnableDepthTest();

	_fbo_GL_RGBA.begin();
	ofClear(0, 0, 0, 0);
	ofBackground(0, 0, 0);
	glShadeModel(GL_SMOOTH);
	ofEnableLighting();

	_fbo_GL_RGBA.end();

	_mainCamera->resetTransform();


	_mainCamera->setPosition(0.0f, 0.0f, 20.0f);
	_mainCamera->setFov(42.5);//60

	//_mainCamera->lookAt(_objModel->getSceneCenter());


	//_objModel->setPosition(ofGetWidth() / 2, ofGetHeight() / 2, 0);

	_objPos = _objModel->getPosition();
	ofLog() << "obj pos " << _objPos;

	_objCenter = _objModel->getSceneCenter();
	ofLog() << "obj center " << _objCenter;

	
	_objModel->getBoundingBox(_objMin, _objMax);
	ofLog() << "obj min " << _objMin << " max " << _objMax;


	// optional normalized scaling
	_objMaxAxis = _objMax.x - _objMin.x;
	_objMaxAxis = MAX(_objMax.y - _objMin.y, _objMaxAxis);
	_objMaxAxis = MAX(_objMax.z - _objMin.z, _objMaxAxis);

	_objMaxAxis *= 1.2f;

	ofLog() << "obj Max Axis " << _objMaxAxis;



	// update camera pos around the obj
	
	_xRotDeg = 0.0;
	_yRotDeg = 0.0;
	_zRotDeg = 0.0;
	_xRot = true;
	_yRot = false;
	_zRot = false;

    _vertical_rotation_degree = 0.0f;
    _horizontal_rotation_degree = 0.0f;
    _scale_distance = 0.30f;
    _camera_rotation_degree = 0.0f;
    _vertical_rotation = false;
    _horizontal_rotation = false;
    _scale = false;
    _camera_rotation = true;

	_pointLight.setDiffuseColor(ofColor(255.f, 255.f, 255.f));
	_pointLight.setSpecularColor(ofColor(10.f, 10.f, 10.f));
	_pointLight.setAmbientColor(ofColor(164.f, 164.f, 164.f));

	_mainCamera->setNearClip(_objMaxAxis * 0.1);

	_mainCamera->setFarClip(_objMaxAxis * 10);

	ofMatrix4x4 projectMatrix = _mainCamera->getProjectionMatrix();
	std::string projectMatrixPath = ofFilePath::join(_workDir, "colorCameraGLProjection.txt");
	projectMatrix.writeToFile(projectMatrixPath);

	std::string volumeDataPath = ofFilePath::join(_workDir, "volumeData.txt");

	ofFile matBuffer;
	matBuffer.open(volumeDataPath, ofFile::WriteOnly, false);
	std::string floatString;
	floatString.append(std::to_string(_objMin.x));
	floatString.append(" ");
	floatString.append(std::to_string(_objMin.y));
	floatString.append(" ");
	floatString.append(std::to_string(_objMin.z));
	floatString.append(" ");
	floatString.append(std::to_string(_objMax.x));
	floatString.append(" ");
	floatString.append(std::to_string(_objMax.y));
	floatString.append(" ");
	floatString.append(std::to_string(_objMax.z));
	floatString.append(" ");

	matBuffer << floatString;

	matBuffer << std::endl;

	matBuffer.close();


}

//--------------------------------------------------------------
void ofApp::update(){

	float xOff = _model_scale * _scale_distance * std::sinf(ofDegToRad(_vertical_rotation_degree)) * std::sinf(ofDegToRad(_horizontal_rotation_degree));//0.0f;
	float yOff = _model_scale * _scale_distance * std::sinf(ofDegToRad(_vertical_rotation_degree)) * std::cosf(ofDegToRad(_horizontal_rotation_degree));//0.0f;
	float zOff = _model_scale * _scale_distance * std::cosf(ofDegToRad(_vertical_rotation_degree));//0.0f;

	_cameraPos.x = _objCenter.x + xOff;
	_cameraPos.y = _objCenter.y + yOff;
	_cameraPos.z = _objCenter.z + zOff;

	_mainCamera->setPosition(_cameraPos.x, _cameraPos.y, _cameraPos.z);


	//_mainCamera->setPosition(0.0f, 0.0f, 10.0f);

	_mainCamera->lookAt(_objCenter);
    //if(_camera_rotation){
    _mainCamera->roll(_camera_rotation_degree);
        //_camera_rotation_degree++;
    //}
	//ofPoint cameraPos = _mainCamera->getGlobalPosition();
	//ofLog() << "camera pos " << cameraPos;

	_pointLight.setPosition(_cameraPos.x, _cameraPos.y, _cameraPos.z);

    if(_camera_rotation){
        _camera_rotation_degree += _camera_rotation_step;
        if (_camera_rotation_degree >= 360.0f)
        {
            _camera_rotation_degree = 0.0f;
            _camera_rotation = false;
            _horizontal_rotation = true;
            _vertical_rotation = false;
            _scale = false;
        }
    }

    if (_horizontal_rotation) {
        //_horizontal_rotation_degree += (15.0f + 345.0f * powf(abs(_vertical_rotation_degree - 90.0f) / 90.0f, 180.0f / (90 - abs(_vertical_rotation_degree - 90.0f) + 1.0f)));
        //_horizontal_rotation_degree += 15.0f;
        if (0 == _vertical_rotation_degree)
        {
            _horizontal_rotation_degree += 360.0f;
        }
        else
        {
            //_horizontal_rotation_degree += (_vertical_rotation_step / (1 - (abs(90.0f - _vertical_rotation_degree) / 90.0f)));
            _horizontal_rotation_degree += _vertical_rotation_step;
        }
        if (_horizontal_rotation_degree >= 360.0f) {
            cout << "horizontal change, new vertical" << endl;
            _horizontal_rotation_degree = 0.0f;
            _camera_rotation = false;
            _horizontal_rotation = false;
            _vertical_rotation = true;
            _scale = false;
        }
        else
        {
            _camera_rotation = true;
            _horizontal_rotation = false;
            _vertical_rotation = false;
            _scale = false;
        }
    }

    if (_vertical_rotation) {
        _vertical_rotation_degree += _vertical_rotation_step;
        if (_vertical_rotation_degree > _vertical_rotation_max)
        {
            cout << "vertical change, new scale" << endl;
            _vertical_rotation_degree = 0.0f;
            _camera_rotation = false;
            _horizontal_rotation = false;
            _vertical_rotation = false;
            _scale = true;
        }
        else
        {
            _camera_rotation = true;
            _horizontal_rotation = false;
            _vertical_rotation = false;
            _scale = false;
        }
    }

    if (_scale) {
        _scale_distance += _scale_step;
        cout << "_scale_distance change, new _scale_distance = " << _scale_distance << endl;

            _camera_rotation = true;
            _horizontal_rotation = false;
            _vertical_rotation = false;
            _scale = false;

    }

    if (_scale_distance > (_scale_max + 0.001f)) {
        cout << "all finished, break" << endl;
        _camera_rotation_degree = 0.0f;
        _horizontal_rotation_degree = 0.0f;
        _vertical_rotation_degree = 0.0f;
        _scale_distance = 0.0f;
        ofExit();
    }
}

//--------------------------------------------------------------
void ofApp::draw(){

	//ofSetColor(ofColor(0.f, 255.f, 0.f));
	//ofClear(0, 0, 0, 0); //
	//_fbo_GL_RGBA.begin();
	//ofEnableDepthTest();
	//ofBackground(0, 0, 0);
	//_mainCamera->begin();
    _fbo_GL_RGBA.begin();
    ofClear(0, 0, 0, 0); //
    ofBackground(0, 0, 0);
    ofEnableDepthTest();
    _mainCamera->begin();

	_pointLight.enable();

	_objModel->drawFaces();

	//ofDrawBox(10);

	//ofDrawBox(0.0f, 0.0f, 0.0f, 2); // middle

	//ofDrawBox(0.0f, 3.0f, 0.0f, 2);  // up

	//ofDrawBox(-3.0f, 0.0f, 0.0f, 2);  // left

	//ofDrawBox(0.0f, -4.0f, 10.0f, 2);  // bottom

	//ofDrawGrid();

	_pointLight.disable();

	_mainCamera->end();

	_fbo_GL_RGBA.end();

	_fbo_GL_RGBA.draw(0.0, 0.0);
	
	saveFrame();
	
	
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

void ofApp::saveFrame()
{
	ofPixels colorPixels;

	_fbo_GL_RGBA.readToPixels(colorPixels);

	std::string colorFile = std::to_string(_frameCount) + ".raw";
	std::string colorPath = ofFilePath::join(_workDir, "gray");

	colorFile = ofFilePath::join(colorPath, colorFile);

	colorPixels.writeToFile(colorFile);

	ofFloatPixels depthPixels;
	_fbo_GL_RGBA.readDepthToPixels(depthPixels);

	float zNear, zFar;
	zNear = _mainCamera->getNearClip();
	zFar = _mainCamera->getFarClip();
	float min = zFar, max = zNear;

	for (auto pixel : depthPixels.getPixelsIter()) {
		float depthInBuffer = pixel[0];


		float depthInClip = (depthInBuffer - 0.5f) * 2.0f;

		float depthInCamera = (2 * zFar * zNear) / (zFar + zNear - depthInClip * (zFar - zNear));

		//float depthInCamera = ((zFar - zNear) / 2)*depthInClip + (zFar + zNear) / 2;

		if (depthInCamera < min) min = depthInCamera;
		if (depthInCamera > max) max = depthInCamera;



		//pixel[0] = (2.0 * zNear) / (zFar + zNear - value * (zFar - zNear));
		pixel[0] = depthInCamera *1000.0f / _model_scale;
	}

	std::string depthFile = std::to_string(_frameCount) + ".raw";
	std::string depthPath = ofFilePath::join(_workDir, "depth");
	depthFile = ofFilePath::join(depthPath, depthFile);

	depthPixels.writeToFile(depthFile);

	// get camera extrinsic matrix 

	ofMatrix4x4 world2camera = _mainCamera->getModelViewMatrix();

	ofMatrix4x4 flipY, flipZ;
	flipY.makeRotationMatrix(180, ofVec3f(0.0f, 0.0f, 1.0f));
	flipZ.makeRotationMatrix(180, ofVec3f(0.0f, 1.0f, 0.0f));

	world2camera *= flipY * flipZ;

	world2camera = ofMatrix4x4::getTransposedOf(world2camera);

	std::string extrinsicFile = std::to_string(_frameCount) + ".txt";
	std::string colorPosPath = ofFilePath::join(_workDir, "pose");
	extrinsicFile = ofFilePath::join(colorPosPath, extrinsicFile);
	world2camera.writeToFile(extrinsicFile);

	// get camera intrinsic matrix
	
	glm::vec3 LookAtDir = _mainCamera->getLookAtDir();
    glm::vec3 UpDir = _mainCamera->getUpDir();
    glm::vec3 SideDir = _mainCamera->getSideDir();
    float XCoords = _mainCamera->getX();
    float YCoords = _mainCamera->getY();
    float ZCoords = _mainCamera->getZ();
    float TT = sqrtf(powf(XCoords, 2.0f) + powf(YCoords, 2.0f) + powf(ZCoords, 2.0f));
    glm::vec3 XAxis = _mainCamera->getXAxis();
    glm::vec3 YAxis = _mainCamera->getYAxis();
    glm::vec3 ZAxis = _mainCamera->getZAxis();
	
    std::string viewInfoFile = std::to_string(_frameCount) + ".txt";
    std::string  viewInfoPath = ofFilePath::join(_workDir, "view");
    viewInfoFile = ofFilePath::join(viewInfoPath, viewInfoFile);
    //world2camera.writeToFile(extrinsicFile);
    ofFile matBuffer;
    matBuffer.open(viewInfoFile, ofFile::WriteOnly, false);
    std::string floatString;
    floatString.append(std::to_string(XAxis.x));
    floatString.append(" ");
    floatString.append(std::to_string(XAxis.y));
    floatString.append(" ");
    floatString.append(std::to_string(XAxis.z));
    floatString.append(" ");
    //matBuffer << floatString;
    //matBuffer << std::endl;
    //floatString.clear();
    floatString.append(std::to_string(YAxis.x));
    floatString.append(" ");
    floatString.append(std::to_string(YAxis.y));
    floatString.append(" ");
    floatString.append(std::to_string(YAxis.z));
    floatString.append(" ");
    //matBuffer << floatString;
    //matBuffer << std::endl;
    //floatString.clear();
    floatString.append(std::to_string(ZAxis.x));
    floatString.append(" ");
    floatString.append(std::to_string(ZAxis.y));
    floatString.append(" ");
    floatString.append(std::to_string(ZAxis.z));
    floatString.append(" ");
    matBuffer << floatString;
    matBuffer << std::endl;

    floatString.clear();
    floatString.append(std::to_string(SideDir.x));
    floatString.append(" ");
    floatString.append(std::to_string(SideDir.y));
    floatString.append(" ");
    floatString.append(std::to_string(SideDir.z));
    floatString.append(" ");
    //matBuffer << floatString;
    //matBuffer << std::endl;
    //floatString.clear();
    floatString.append(std::to_string(UpDir.x));
    floatString.append(" ");
    floatString.append(std::to_string(UpDir.y));
    floatString.append(" ");
    floatString.append(std::to_string(UpDir.z));
    floatString.append(" ");
    //matBuffer << floatString;
    //matBuffer << std::endl;
    //floatString.clear();
    floatString.append(std::to_string(LookAtDir.x));
    floatString.append(" ");
    floatString.append(std::to_string(LookAtDir.y));
    floatString.append(" ");
    floatString.append(std::to_string(LookAtDir.z));
    floatString.append(" ");
    matBuffer << floatString;
    matBuffer << std::endl;
    floatString.clear();
    floatString.append(std::to_string(TT));
    matBuffer << floatString;
    matBuffer << std::endl;
    matBuffer.close();

	_frameCount++;
}

void ofApp::setScanInput(string file)
{
	_inputModel = file;
}

void ofApp::setScanOutput(string path)
{
	_workDir = path;
}

void ofApp::setScanParam(string str_param)
{
    vector<float> vfParam;
    const char *p = str_param.c_str();
    while (p && *p != '\0')
    {
        float d = atof(p);
        vfParam.push_back(d);
        while (*p != ','&& *p != '\0') p++;
        if (*p == '\0') break;
        p++;
    }
    if (vfParam.size() == 6)
    {
        _camera_rotation_step = float(vfParam[0]);
        _vertical_rotation_step = float(vfParam[1]);
        _vertical_rotation_max = float(vfParam[2]);
        _scale_step = float(vfParam[3]);
        _scale_max = float(vfParam[4]);
        _model_scale = float(vfParam[5]);
        //_scale_distance *= _model_scale;
        //_scale_step *= _model_scale;
        //_scale_max *= _model_scale;
    }
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
