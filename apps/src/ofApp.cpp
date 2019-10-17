#include "ofApp.h"
#include "ofxAssimpModelLoader.h"

GLfloat lightOnePosition[] = { 40.0, 40, 100.0, 0.0 };
GLfloat lightOneColor[] = { 0.99, 0.99, 0.99, 1.0 };

GLfloat lightTwoPosition[] = { -40.0, 40, 100.0, 0.0 };
GLfloat lightTwoColor[] = { 0.99, 0.99, 0.99, 1.0 };

ofApp::ofApp()
{
	obj_model_ = nullptr;
	main_camera_ = nullptr;
	frame_count_ = 0;
	model_scale_ = 1.0f;
	n_points_ = 1500;
	n_sphere_fraction_ = 16;
	input_model_ = std::string();
	work_dir_ = std::string();
	extrinsics_param_ = std::string();
	input_param_ = std::string();
}

ofApp::~ofApp()
{
	if (obj_model_) delete obj_model_;
	obj_model_ = nullptr;

	if (main_camera_) delete main_camera_;
	main_camera_ = nullptr;
}

//--------------------------------------------------------------
void ofApp::setup(){

	if (!obj_model_) {
		obj_model_ = new ofxAssimpModelLoader();
	}

	if (!main_camera_) {
		main_camera_ = new ofCamera();
	}
	obj_model_->setScaleNormalization(false);
	
	if (!obj_model_->loadModel(input_model_, 20)) {
		ofExit(2);
	}
	std::string model_fix_path = ofFilePath::join(work_dir_, "model.obj");
	obj_model_->saveModel(model_fix_path);
	ofFbo::Settings fboSettings;
	fboSettings.width = ofGetWidth();
	fboSettings.height = ofGetHeight();
	fboSettings.internalformat = GL_RGBA;
	fboSettings.numSamples = 0;
	fboSettings.depthStencilAsTexture = false;

	//fbo_gl_rgba_.allocate(fboSettings);
	fbo_gl_rgba_.allocate(ofGetWidth(), ofGetHeight(), GL_RGBA, 0);

	ofEnableDepthTest();

	fbo_gl_rgba_.begin();
	ofClear(0, 0, 0, 0);
	ofBackground(0, 0, 0);
	glShadeModel(GL_SMOOTH);
	ofEnableLighting();

	fbo_gl_rgba_.end();

	main_camera_->resetTransform();

	main_camera_->setFov(60);//f = 579.4

	obj_pos_ = obj_model_->getPosition();
	ofLog() << "obj pos " << obj_pos_;

	obj_center_ = obj_model_->getSceneCenter();
	ofLog() << "obj center " << obj_center_;

	
	obj_model_->getBoundingBox(obj_min_, obj_max_);
	ofLog() << "obj min " << obj_min_ << " max " << obj_max_;


	// optional normalized scaling
	obj_max_axis_ = obj_max_.x - obj_min_.x;
	obj_max_axis_ = MAX(obj_max_.y - obj_min_.y, obj_max_axis_);
	obj_max_axis_ = MAX(obj_max_.z - obj_min_.z, obj_max_axis_);

	obj_max_axis_ *= 1.2f;

	ofLog() << "obj Max Axis " << obj_max_axis_;



	// update camera pos around the obj
    cv_pi_ = 3.1415926f;
    index_ = 0;
    angle_ = 0; angle_min_ = 0; angle_max_ = 360; angle_step_ = 15;
	radius_step_ = customer_defined_radius_ * 0.08;
	radius_min_ = customer_defined_radius_ - 2 * radius_step_;
	radius_max_ = customer_defined_radius_ + 2 * radius_step_;
	radius_ = radius_min_;
    

	point_light_.setDiffuseColor(ofColor(255.f, 255.f, 255.f));
	point_light_.setSpecularColor(ofColor(10.f, 10.f, 10.f));
	point_light_.setAmbientColor(ofColor(164.f, 164.f, 164.f));

	main_camera_->setNearClip(obj_max_axis_ * 0.1);
	//main_camera_->setNearClip(0.1f);

	main_camera_->setFarClip(obj_max_axis_ * 10);
	//main_camera_->setFarClip(256.0f);

	ofMatrix4x4 projectMatrix = main_camera_->getProjectionMatrix();
	std::string projectMatrixPath = ofFilePath::join(work_dir_, "colorCameraGLProjection.txt");
	projectMatrix.writeToFile(projectMatrixPath);

	std::string volumeDataPath = ofFilePath::join(work_dir_, "volumeData.txt");

	ofFile matBuffer;
	matBuffer.open(volumeDataPath, ofFile::WriteOnly, false);
	std::string floatString;
	floatString.append(std::to_string(obj_min_.x));
	floatString.append(" ");
	floatString.append(std::to_string(obj_min_.y));
	floatString.append(" ");
	floatString.append(std::to_string(obj_min_.z));
	floatString.append(" ");
	floatString.append(std::to_string(obj_max_.x));
	floatString.append(" ");
	floatString.append(std::to_string(obj_max_.y));
	floatString.append(" ");
	floatString.append(std::to_string(obj_max_.z));
	floatString.append(" ");
	matBuffer << floatString;
	matBuffer << std::endl;
	matBuffer.close();


	std::string renderInfoPath = ofFilePath::join(work_dir_, "render_info.txt");
	matBuffer.open(renderInfoPath, ofFile::WriteOnly, false);
	floatString.clear();
	floatString.append("FOV: ");
	floatString.append(std::to_string(main_camera_->getFov()));
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
	floatString.append(std::to_string(model_scale_));
	matBuffer << floatString;
	matBuffer << std::endl;
	floatString.clear();
	floatString.append("EXTRINSIC: ");
	floatString.append(extrinsics_param_);
	matBuffer << floatString;
	matBuffer << std::endl;
	floatString.clear();
	floatString.append("MODEL_SCALE: ");
	floatString.append(std::to_string(model_scale_));
	matBuffer << floatString;
	matBuffer << std::endl;
	floatString.clear();
	floatString.append("RENDER_POINT: ");
	floatString.append(std::to_string(n_points_));
	matBuffer << floatString;
	matBuffer << std::endl;
	floatString.clear();
	floatString.append("SPHERE_FRACTION: 1/");
	floatString.append(std::to_string(n_sphere_fraction_));
	matBuffer << floatString;
	matBuffer << std::endl;
	matBuffer.close();

	
}

//--------------------------------------------------------------
void ofApp::update(){

    float angle_rad = cv_pi_ * angle_  / 180.0f;

    static float inc = cv_pi_ * (3 - sqrt(5));
    static float off = 2.0f / float(n_points_);
    float z = index_ * off - 1.0f + (off / 2.0f);
    //z = -z;
    float r = sqrt(1.0f - z * z);
    float phi = index_ * inc;
    float y = std::cos(phi) * r;
    float x = std::sin(phi) * r;
	// cv_module
	float xOff = model_scale_ * x * radius_;
	float yOff = model_scale_ * y * radius_;
	float zOff = model_scale_ * z * radius_;

	
	glm::vec3 t = glm::vec3(xOff, yOff, zOff);
	glm::vec3 C = gl_rotate_matrix_ * t;// = -R * t;

	camera_pos_.x = obj_center_.x + C.x;
	camera_pos_.y = obj_center_.y + C.y;
	camera_pos_.z = obj_center_.z + C.z;
	main_camera_->resetTransform();
	main_camera_->setPosition(camera_pos_.x, camera_pos_.y, camera_pos_.z);

	main_camera_->lookAt(obj_center_);
    
    main_camera_->roll(angle_);
	
	point_light_.setPosition(camera_pos_.x, camera_pos_.y, camera_pos_.z);

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
	//fbo_gl_rgba_.begin();
	//ofEnableDepthTest();
	//ofBackground(0, 0, 0);
	//main_camera_->begin();
    fbo_gl_rgba_.begin();
    ofClear(0, 0, 0, 0); //
    ofBackground(0, 0, 0);
    ofEnableDepthTest();
    main_camera_->begin();

	point_light_.enable();

	obj_model_->drawFaces();

	//ofDrawBox(10);

	//ofDrawBox(0.0f, 0.0f, 0.0f, 2); // middle

	//ofDrawBox(0.0f, 3.0f, 0.0f, 2);  // up

	//ofDrawBox(-3.0f, 0.0f, 0.0f, 2);  // left

	//ofDrawBox(0.0f, -4.0f, 10.0f, 2);  // bottom

	//ofDrawGrid();

	point_light_.disable();

	main_camera_->end();

	fbo_gl_rgba_.end();

	fbo_gl_rgba_.draw(0.0, 0.0);
	
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

	fbo_gl_rgba_.readToPixels(colorPixels);

	std::string colorFile = std::to_string(frame_count_) + ".raw";
	std::string colorPath = ofFilePath::join(work_dir_, "gray");

	colorFile = ofFilePath::join(colorPath, colorFile);

	colorPixels.writeToFile(colorFile);

	ofFloatPixels depthPixels;
	fbo_gl_rgba_.readDepthToPixels(depthPixels);

	float zNear, zFar;
	zNear = main_camera_->getNearClip();
	zFar = main_camera_->getFarClip();
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

	std::string depthFile = std::to_string(frame_count_) + ".raw";
	std::string depthPath = ofFilePath::join(work_dir_, "depth");
	depthFile = ofFilePath::join(depthPath, depthFile);

	depthPixels.writeToFile(depthFile);

	// get camera extrinsic matrix 

	ofMatrix4x4 world2camera = main_camera_->getModelViewMatrix();

	ofMatrix4x4 flipY, flipZ;
	flipY.makeRotationMatrix(180, ofVec3f(0.0f, 0.0f, 1.0f));
	flipZ.makeRotationMatrix(180, ofVec3f(0.0f, 1.0f, 0.0f));
	ofMatrix4x4 a = flipY * flipZ;

	world2camera *= flipY * flipZ;

	world2camera = ofMatrix4x4::getTransposedOf(world2camera);

	std::string extrinsicFile = std::to_string(frame_count_) + ".txt";
	std::string colorPosPath = ofFilePath::join(work_dir_, "pose");
	extrinsicFile = ofFilePath::join(colorPosPath, extrinsicFile);
	world2camera.writeToFile(extrinsicFile);

	frame_count_++;
}

void ofApp::setScanInput(string file)
{
	input_model_ = file;
}

void ofApp::setScanOutput(string path)
{
	work_dir_ = path;
}

void ofApp::setScanParam(string str_param)
{
	input_param_ = str_param;
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
        model_scale_ = float(vfParam[0]);
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
	extrinsics_param_ = str_param;
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
		float angle_rad = cv_pi_ * angle_ / 180.0f;

		static float inc = cv_pi_ * (3 - sqrt(5));
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
		ofPoint C_norm = ofPoint(C.x - obj_center_.x, C.y - obj_center_.y, C.z - obj_center_.z);
		customer_defined_radius_ = C_norm.length();
		C_norm.normalize();
		ofPoint C_ori = ofPoint(x, y, z);
		ofLog() << "C_ori: " << C_ori;
		ofLog() << "C_norm: " << C_norm;

		ofMatrix4x4 gl_rotate;
		gl_rotate.makeRotationMatrix(C_ori, C_norm);
		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				gl_rotate_matrix_[i][j] = gl_rotate._mat[i][j];
			}
		}
	}
}
//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
