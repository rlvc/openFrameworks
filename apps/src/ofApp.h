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
		ofxAssimpModelLoader * obj_model_;
		ofPoint obj_pos_, obj_center_, obj_min_, obj_max_;
		float obj_max_axis_;
		ofCamera* main_camera_;
		ofPoint camera_pos_;

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
        float cv_pi_;
		glm::mat3 gl_rotate_matrix_;

		ofLight point_light_;

		ofFbo fbo_gl_rgba_;

		int frame_count_;

		std::string input_model_;
		std::string work_dir_;
		std::string extrinsics_param_;
		std::string input_param_;

        float model_scale_;
		
};
