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
	_model_scale = 1.0f;
	n_points_ = 1500;
	//n_points_ = 150;//_lessimage
	n_sphere_fraction_ = 16;
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
	std::string model_fix_path = ofFilePath::join(_workDir, "model.obj");
	_objModel->saveModel(model_fix_path);
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

	_mainCamera->setFov(60);//f = 579.4

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
    CV_PI = 3.1415926f;
    index_ = 0;
    angle_ = 0; angle_min_ = 0; angle_max_ = 360; angle_step_ = 15;
	radius_step_ = customer_defined_radius_ * 0.08;
	radius_min_ = customer_defined_radius_ - 2 * radius_step_;
	radius_max_ = customer_defined_radius_ + 2 * radius_step_;
	radius_ = radius_min_;
    

	_pointLight.setDiffuseColor(ofColor(255.f, 255.f, 255.f));
	_pointLight.setSpecularColor(ofColor(10.f, 10.f, 10.f));
	_pointLight.setAmbientColor(ofColor(164.f, 164.f, 164.f));

	_mainCamera->setNearClip(_objMaxAxis * 0.1);
	//_mainCamera->setNearClip(0.1f);

	_mainCamera->setFarClip(_objMaxAxis * 10);
	//_mainCamera->setFarClip(256.0f);

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


	std::string renderInfoPath = ofFilePath::join(_workDir, "render_info.txt");
	matBuffer.open(renderInfoPath, ofFile::WriteOnly, false);
	floatString.clear();
	floatString.append("FOV: ");
	floatString.append(std::to_string(_mainCamera->getFov()));
	matBuffer << floatString;
	matBuffer << std::endl;
	floatString.clear();
	floatString.append("WIDTH: ");
	floatString.append(std::to_string(fboSettings.width));
	matBuffer << floatString;
	matBuffer << std::endl;
	floatString.clear();
	floatString.append("HEIGHT: ");
	floatString.append(std::to_string(fboSettings.height));
	matBuffer << floatString;
	matBuffer << std::endl;
	floatString.clear();
	floatString.append("MODEL_SCALE: ");
	floatString.append(std::to_string(_model_scale));
	matBuffer << floatString;
	matBuffer << std::endl;
	matBuffer.close();

	
}

//--------------------------------------------------------------
void ofApp::update(){

    float angle_rad = CV_PI * angle_  / 180.0f;

    static float inc = CV_PI * (3 - sqrt(5));
    static float off = 2.0f / float(n_points_);
    float z = index_ * off - 1.0f + (off / 2.0f);
    //z = -z;
    float r = sqrt(1.0f - z * z);
    float phi = index_ * inc;
    float y = std::cos(phi) * r;
    float x = std::sin(phi) * r;
	// cv_module
	float xOff = _model_scale * x * radius_;
	float yOff = _model_scale * y * radius_;
	float zOff = _model_scale * z * radius_;

	
	glm::vec3 t = glm::vec3(xOff, yOff, zOff);
	glm::vec3 C = glR * t;// = -R * t;

	_cameraPos.x = _objCenter.x + C.x;
	_cameraPos.y = _objCenter.y + C.y;
	_cameraPos.z = _objCenter.z + C.z;
	_mainCamera->resetTransform();
	_mainCamera->setPosition(_cameraPos.x, _cameraPos.y, _cameraPos.z);

	_mainCamera->lookAt(_objCenter);
    
    _mainCamera->roll(angle_);
	
	_pointLight.setPosition(_cameraPos.x, _cameraPos.y, _cameraPos.z);

    angle_ += angle_step_;
    if (angle_ >= angle_max_)
    {
        angle_ = angle_min_;
        radius_ += radius_step_;
        if (radius_ > radius_max_ + radius_step_ / 2)
        {
            radius_ = radius_min_;
            ++index_;
        }
    }
    if (index_ >= n_points_ / n_sphere_fraction_) {
        cout << "all finished, break" << endl;
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
		pixel[0] = depthInCamera;
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
	ofMatrix4x4 a = flipY * flipZ;

	world2camera *= flipY * flipZ;

	world2camera = ofMatrix4x4::getTransposedOf(world2camera);

	std::string extrinsicFile = std::to_string(_frameCount) + ".txt";
	std::string colorPosPath = ofFilePath::join(_workDir, "pose");
	extrinsicFile = ofFilePath::join(colorPosPath, extrinsicFile);
	world2camera.writeToFile(extrinsicFile);

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
    if (vfParam.size() >= 1)
    {
        _model_scale = float(vfParam[0]);
    }
	if (vfParam.size() >= 2)
	{
		n_points_ = (size_t)(vfParam[1]);
	}
	if (vfParam.size() >= 3)
	{
		n_sphere_fraction_ = (size_t)(vfParam[2]);
	}
}

void ofApp::setExtrinsicsParam(string str_param)
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
	if (vfParam.size() == 16)
	{
		float angle_rad = CV_PI * angle_ / 180.0f;

		static float inc = CV_PI * (3 - sqrt(5));
		static float off = 2.0f / float(n_points_);
		float z = 0 * off - 1.0f + (off / 2.0f);
		//z = -z;
		float r = sqrt(1.0f - z * z);
		float phi = 0 * inc;
		float y = std::cos(phi) * r;
		float x = std::sin(phi) * r;

		//float fext[16] = { 1.0 ,0.0, 0.0, 8.403056, 0.0, -0.036511, 0.999333, 100.035164,-0.0, -0.999333, - 0.036511, 544.574646, 0.0, 0.0, 0.0, 1.0 };
		glm::mat4 mat4_ext;
		mat4_ext[0][0] = vfParam[0];
		mat4_ext[1][0] = vfParam[1];
		mat4_ext[2][0] = vfParam[2];
		mat4_ext[3][0] = vfParam[3];
		mat4_ext[0][1] = vfParam[4];
		mat4_ext[1][1] = vfParam[5];
		mat4_ext[2][1] = vfParam[6];
		mat4_ext[3][1] = vfParam[7];
		mat4_ext[0][2] = vfParam[8];
		mat4_ext[1][2] = vfParam[9];
		mat4_ext[2][2] = vfParam[10];
		mat4_ext[3][2] = vfParam[11];
		mat4_ext[0][3] = vfParam[12];
		mat4_ext[1][3] = vfParam[13];
		mat4_ext[2][3] = vfParam[14];
		mat4_ext[3][3] = vfParam[15];
		ofMatrix4x4 world2camera = glm::inverse(mat4_ext);

		ofMatrix4x4 flipY, flipZ;
		flipY.makeRotationMatrix(180, ofVec3f(0.0f, 0.0f, 1.0f));
		flipZ.makeRotationMatrix(180, ofVec3f(0.0f, 1.0f, 0.0f));
		world2camera *= flipY * flipZ;
		world2camera = ofMatrix4x4::getTransposedOf(world2camera);

		glm::mat3 R;
		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				R[i][j] = world2camera._mat[i][j];
			}
		}
		glm::vec3 t = glm::vec3(world2camera._mat[0][3], world2camera._mat[1][3], world2camera._mat[2][3]);
		glm::vec3 C = -R * t;
		ofPoint C_norm = ofPoint(C.x - _objCenter.x, C.y - _objCenter.y, C.z - _objCenter.z);
		customer_defined_radius_ = C_norm.length();
		C_norm.normalize();
		ofPoint C_ori = ofPoint(x, y, z);
		ofLog() << "C_ori: " << C_ori;
		ofLog() << "C_norm: " << C_norm;

		glRotateMatrix.makeRotationMatrix(C_ori, C_norm);
		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				glR[i][j] = glRotateMatrix._mat[i][j];
			}
		}
	}
}
//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
