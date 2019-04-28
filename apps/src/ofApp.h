#pragma once

#include "ofMain.h"

class ofxAssimpModelLoader;
class ofCamera;

class ofApp : public ofBaseApp{

	public:
		ofApp();
		virtual ~ofApp();
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

		void saveFrame();

		void setScanInput(string file);
		void setScanOutput(string path);
        void setScanParam(string str_param);

    private:
		ofxAssimpModelLoader * _objModel;
		ofPoint _objPos, _objCenter, _objMin, _objMax;
		float _objMaxAxis;
		ofCamera* _mainCamera;
		ofPoint _cameraPos;
		float _xRotDeg, _yRotDeg, _zRotDeg;
		bool _xRot, _yRot, _zRot;

        float _vertical_rotation_degree, _horizontal_rotation_degree, _scale_distance;
        float _camera_rotation_degree;
        bool _vertical_rotation, _horizontal_rotation, _scale, _camera_rotation;

		ofLight _pointLight;

		ofFbo _fbo_GL_RGBA;

		int _frameCount;

		std::string _inputModel;
		std::string _workDir;

        float _camera_rotation_step, _vertical_rotation_step, _scale_step;
        float _vertical_rotation_max, _scale_max;
        float _model_scale;
		
};
