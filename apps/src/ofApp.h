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
        void setExtrinsicsParam(string str_param);

    private:
		ofxAssimpModelLoader * _objModel;
		ofPoint _objPos, _objCenter, _objMin, _objMax;
		float _objMaxAxis;
		ofCamera* _mainCamera;
		ofPoint _cameraPos;

        /** The number of points on the sphere */
        size_t n_points_;
		/** The fraction of the sphere need to render */
		size_t n_sphere_fraction_;
        /** The index of the view point we are at now */
        size_t index_;
        /** Values for the angle sampling in degrees */
        int angle_min_, angle_max_, angle_step_, angle_;
        /** Values for the scale sampling */
		float customer_defined_radius_;
        float radius_min_, radius_max_, radius_step_, radius_;
        float CV_PI;
		ofMatrix4x4 glRotateMatrix;
		glm::mat3 glR;

		ofLight _pointLight;

		ofFbo _fbo_GL_RGBA;

		int _frameCount;

		std::string _inputModel;
		std::string _workDir;

        float _model_scale;
		
};
