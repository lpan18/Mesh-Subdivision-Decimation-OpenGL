/*
    Sample code by Wallace Lira <http://www.sfu.ca/~wpintoli/> based on
    the four Nanogui examples and also on the sample code provided in
          https://github.com/darrenmothersele/nanogui-test
    
    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/

#include <nanogui/opengl.h>
#include <nanogui/glutil.h>
#include <nanogui/screen.h>
#include <nanogui/window.h>
#include <nanogui/layout.h>
#include <nanogui/label.h>
#include <nanogui/checkbox.h>
#include <nanogui/button.h>
#include <nanogui/toolbutton.h>
#include <nanogui/popupbutton.h>
#include <nanogui/combobox.h>
#include <nanogui/progressbar.h>
#include <nanogui/entypo.h>
#include <nanogui/messagedialog.h>
#include <nanogui/textbox.h>
#include <nanogui/slider.h>
#include <nanogui/imagepanel.h>
#include <nanogui/imageview.h>
#include <nanogui/vscrollpanel.h>
#include <nanogui/colorwheel.h>
#include <nanogui/graph.h>
#include <nanogui/tabwidget.h>
#include <nanogui/glcanvas.h>
#include <iostream>
#include <string>

// Includes for the GLTexture class.
#include <algorithm>
#include <cstdint>
#include <memory>
#include <utility>


#if defined(__GNUC__)
#  pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#endif
#if defined(_WIN32)
#  pragma warning(push)
#  pragma warning(disable: 4457 4456 4005 4312)
#endif

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#if defined(_WIN32)
#  pragma warning(pop)
#endif
#if defined(_WIN32)
#  if defined(APIENTRY)
#    undef APIENTRY
#  endif
#  include <windows.h>
#endif

#include "WingedEdge.h"

using std::cout;
using std::cerr;
using std::endl;
using std::string;
using std::vector;
using std::pair;
using std::to_string;

using nanogui::Screen;
using nanogui::Window;
using nanogui::GroupLayout;
using nanogui::Button;
using nanogui::CheckBox;
using nanogui::Vector2f;
using nanogui::Vector2i;
using nanogui::Vector3f;
using nanogui::MatrixXu;
using nanogui::MatrixXf;
using nanogui::Label;
using nanogui::Arcball;

class MyGLCanvas : public nanogui::GLCanvas {
public:
    MyGLCanvas(Widget *parent) : nanogui::GLCanvas(parent) {
        using namespace nanogui;

	mShader.initFromFiles("a_smooth_shader", "StandardShading.vertexshader", "StandardShading.fragmentshader");
 
	/* which are the 3 dimensions and 8 possible vertice positions */
        positions.col(0) <<  1,  1, -1; //top right back
        positions.col(1) << -1,  1, -1; //bottom right back
        positions.col(2) << -1,  1, 1; //bottom right front

	positions.col(3) <<  1,  1, -1; //top right back
	positions.col(4) << -1,  1, 1; //bottom right front
        positions.col(5) <<  1,  1, 1; //top right front

	positions.col(6) <<  1,  1, -1; //top right back
	positions.col(7) <<  1, -1, -1; //top left back
        positions.col(8) << -1, -1, -1; //bottom left back

	positions.col(9) <<  1,  1, -1; //top right back
	positions.col(10) << -1, -1, -1; //bottom left back
	positions.col(11) << -1,  1, -1; //bottom right back

	positions.col(12) << -1,  1, -1; //bottom right back
	positions.col(13) << -1, -1, -1; //bottom left back
	positions.col(14) << -1, -1, 1; //bottom left front

	positions.col(15) << -1,  1, -1; //bottom right back
	positions.col(16) << -1, -1, 1; //bottom left front
	positions.col(17) << -1,  1, 1; //bottom right front

	positions.col(18) << -1,  1, 1; //bottom right front
	positions.col(19) << -1, -1, 1; //bottom left front
        positions.col(20) <<  1, -1, 1; //top left front

	positions.col(21) << -1,  1, 1; //bottom right front
        positions.col(22) <<  1, -1, 1; //top left front
	positions.col(23) <<  1,  1, 1; //top right front

	positions.col(24) <<  1,  1, 1; //top right front
	positions.col(25) <<  1, -1, 1; //top left front
	positions.col(26) <<  1, -1, -1; //top left back
	
	positions.col(27) <<  1,  1, 1; //top right front
	positions.col(28) <<  1, -1, -1; //top left back
	positions.col(29) <<  1,  1, -1; //top right back

	positions.col(30) <<  1, -1, -1; //top left back
	positions.col(31) <<  1, -1, 1; //top left front
	positions.col(32) << -1, -1, 1; //bottom left front

	positions.col(33) <<  1, -1, -1; //top left back
	positions.col(34) << -1, -1, 1; //bottom left front
	positions.col(35) << -1, -1, -1; //bottom left back

	//lines
	positions.col(36) <<  1,  1, -1; //top right back
        positions.col(37) << -1,  1, -1; //bottom right back
	
	positions.col(38) << -1,  1, -1; //bottom right back
        positions.col(39) << -1,  1, 1; //bottom right front

	positions.col(40) << -1,  1, 1; //bottom right front
	positions.col(41) <<  1,  1, -1; //top right back

	positions.col(42) <<  1,  1, -1; //top right back
	positions.col(43) << -1,  1, 1; //bottom right front

	positions.col(44) << -1,  1, 1; //bottom right front
        positions.col(45) <<  1,  1, 1; //top right front
	
	positions.col(46) <<  1,  1, -1; //top right back
        positions.col(47) <<  1,  1, 1; //top right front
	
	
	//notice that the line is jagged. You actually need to move them slightly out
	//To do that on an arbitrary mesh, just use the surface normals to your advantage!
	/*
	positions.col(36) <<  1.005,  1.005, -1.005; //top right back
        positions.col(37) << -1.005,  1.005, -1.005; //bottom right back
	
	positions.col(38) << -1.005,  1.005, -1.005; //bottom right back
        positions.col(39) << -1.005,  1.005, 1.005; //bottom right front

	positions.col(40) << -1.005,  1.005, 1.005; //bottom right front
	positions.col(41) <<  1.005,  1.005, -1.005; //top right back

	positions.col(42) <<  1.005,  1.005, -1.005; //top right back
	positions.col(43) << -1.005,  1.005, 1.005; //bottom right front

	positions.col(44) << -1.005,  1.005, 1.005; //bottom right front
        positions.col(45) <<  1.005,  1.005, 1.005; //top right front
	
	positions.col(46) <<  1.005,  1.005, -1.005; //top right back
        positions.col(47) <<  1.005,  1.005, 1.005; //top right front
	*/
	

	/* these are the vertex normals, normally you need to calculate this using face normals*/
	/* in turn, to calculate a face normal you need the cross product of face points*/
	/* this trick only works because it is a simple cube centered at the origin */
        normals.col( 0) << Vector3f(1,1,-1).normalized();
        normals.col( 1) << Vector3f(-1,1,-1).normalized();
        normals.col( 2) << Vector3f(-1,1,1).normalized();

	normals.col( 3) << Vector3f(1,1,-1).normalized();
        normals.col( 4) << Vector3f(-1,1,1).normalized();
	normals.col( 5) << Vector3f(1,1,1).normalized();

	normals.col( 6) << Vector3f(1,1,-1).normalized();
	normals.col( 7) << Vector3f(1,-1,-1).normalized();
        normals.col( 8) << Vector3f(-1,-1,-1).normalized();

	normals.col( 9) << Vector3f(1,1,-1).normalized();
        normals.col( 10) << Vector3f(-1,-1,-1).normalized();
	normals.col( 11) << Vector3f(-1,1,-1).normalized();

	normals.col( 12) << Vector3f(-1,1,-1).normalized();
        normals.col( 13) << Vector3f(-1,-1,-1).normalized();
	normals.col( 14) << Vector3f(-1,-1,1).normalized();

	normals.col( 15) << Vector3f(-1,1,-1).normalized();
	normals.col( 16) << Vector3f(-1,-1,1).normalized();
        normals.col( 17) << Vector3f(-1,1,1).normalized();

        normals.col( 18) << Vector3f(-1,1,1).normalized();
	normals.col( 19) << Vector3f(-1,-1,1).normalized();
	normals.col( 20) << Vector3f(1,-1,1).normalized();

	normals.col( 21) << Vector3f(-1,1,1).normalized();
	normals.col( 22) << Vector3f(1,-1,1).normalized();
	normals.col( 23) << Vector3f(1,1,1).normalized();

	normals.col( 24) << Vector3f(1,1,1).normalized();
	normals.col( 25) << Vector3f(1,-1,1).normalized();
	normals.col( 26) << Vector3f(1,-1,-1).normalized();

	normals.col( 27) << Vector3f(1,1,1).normalized();
	normals.col( 28) << Vector3f(1,-1,-1).normalized();
	normals.col( 29) << Vector3f(1,1,-1).normalized();

	normals.col( 30) << Vector3f(1,-1,-1).normalized();
	normals.col( 31) << Vector3f(1,-1,1).normalized();
	normals.col( 32) << Vector3f(-1,-1,1).normalized();

	normals.col( 33) << Vector3f(1,-1,-1).normalized();
	normals.col( 34) << Vector3f(-1,-1,1).normalized();
	normals.col( 35) << Vector3f(-1,-1,-1).normalized();

	//line normals
	normals.col( 36) << Vector3f(1,1,-1).normalized();
        normals.col( 37) << Vector3f(-1,1,-1).normalized();

        normals.col( 38) << Vector3f(-1,1,-1).normalized();
        normals.col( 39) << Vector3f(-1,1,1).normalized();

	normals.col( 40) << Vector3f(1,1,-1).normalized();
        normals.col( 41) << Vector3f(-1,1,1).normalized();

	normals.col( 42) << Vector3f(1,1,-1).normalized();
        normals.col( 43) << Vector3f(-1,1,1).normalized();

	normals.col( 44) << Vector3f(-1,1,1).normalized();
	normals.col( 45) << Vector3f(1,1,1).normalized();

	normals.col( 46) << Vector3f(1,1,-1).normalized();
	normals.col( 47) << Vector3f(1,1,1).normalized();

        /* Each vertice can have a color too (rgb in this case) */
	/* the face will interpolate the color of its vertices to create a gradient */
	//just a red cube by default
	colors.col( 0) << 1, 0, 0;
        colors.col( 1) << 1, 0, 0;
        colors.col( 2) << 1, 0, 0;
        colors.col( 3) << 1, 0, 0;
        colors.col( 4) << 1, 0, 0;
        colors.col( 5) << 1, 0, 0;
        colors.col( 6) << 1, 0, 0;
        colors.col( 7) << 1, 0, 0;
        colors.col( 8) << 1, 0, 0;
        colors.col( 9) << 1, 0, 0;
        colors.col(10) << 1, 0, 0;
        colors.col(11) << 1, 0, 0;
        colors.col(12) << 1, 0, 0;
        colors.col(13) << 1, 0, 0;
        colors.col(14) << 1, 0, 0;
        colors.col(15) << 1, 0, 0;
        colors.col(16) << 1, 0, 0;
        colors.col(17) << 1, 0, 0;
        colors.col(18) << 1, 0, 0;
        colors.col(19) << 1, 0, 0;
        colors.col(20) << 1, 0, 0;
        colors.col(21) << 1, 0, 0;
        colors.col(22) << 1, 0, 0;
        colors.col(23) << 1, 0, 0;
        colors.col(24) << 1, 0, 0;
        colors.col(25) << 1, 0, 0;
        colors.col(26) << 1, 0, 0;
        colors.col(27) << 1, 0, 0;
        colors.col(28) << 1, 0, 0;
        colors.col(29) << 1, 0, 0;
        colors.col(30) << 1, 0, 0;
        colors.col(31) << 1, 0, 0;
        colors.col(32) << 1, 0, 0;
        colors.col(33) << 1, 0, 0;
        colors.col(34) << 1, 0, 0;
        colors.col(35) << 1, 0, 0;

	//line colors
        colors.col(36) << 0, 0, 0;
        colors.col(37) << 0, 0, 0;
        colors.col(38) << 0, 0, 0;
        colors.col(39) << 0, 0, 0;
        colors.col(40) << 0, 0, 0;
        colors.col(41) << 0, 0, 0;
        colors.col(42) << 0, 0, 0;
        colors.col(43) << 0, 0, 0;
        colors.col(44) << 0, 0, 0;
        colors.col(45) << 0, 0, 0;
        colors.col(46) << 0, 0, 0;
        colors.col(47) << 0, 0, 0;

	// After binding the shader to the current context we can send data to opengl that will be handled
	// by the vertex shader and then by the fragment shader, in that order.
	// if you want to know more about modern opengl pipeline take a look at this link
	// https://www.khronos.org/opengl/wiki/Rendering_Pipeline_Overview
        mShader.bind();

        //mShader.uploadIndices(indices);
        mShader.uploadAttrib("vertexPosition_modelspace", positions);
        mShader.uploadAttrib("color", colors);
	mShader.uploadAttrib("vertexNormal_modelspace", normals);

	// ViewMatrixID
	// change your rotation to work on the camera instead of rotating the entire world with the MVP matrix
	Matrix4f V;
	V.setIdentity();
	V = lookAt(Vector3f(0,12,0), Vector3f(0,0,0), Vector3f(0,1,0));
	mShader.setUniform("V", V);

	//ModelMatrixID
	Matrix4f M;
	M.setIdentity();
	mShader.setUniform("M", M);
	
	// This the light origin position in your environment, which is totally arbitrary
	// however it is better if it is behind the observer
	mShader.setUniform("LightPosition_worldspace", Vector3f(-2,6,-4));

    }

    //flush data on call
    ~MyGLCanvas() {
        mShader.free();
    }

    void loadObj(string fileName) {
        mWe = new WingedEdge(fileName);
    }

    //Method to update the rotation on each axis
    void setRotation(nanogui::Vector3f vRotation) {
        mRotation = vRotation;
    }

    //Method to update the rotation on each axis
    void setTranslation(nanogui::Vector3f vTranslation) {
        mTranslation = vTranslation;
    }

    void setZooming(float fZooming) {
        mZooming = fZooming;
    }

    //Method to update the mesh itself, can change the size of it dynamically, as shown later
    void updateMeshPositions(MatrixXf newPositions){
        positions = newPositions;

    }

    //OpenGL calls this method constantly to update the screen.
    virtual void drawGL() override {
        using namespace nanogui;

	//refer to the previous explanation of mShader.bind();
        mShader.bind();

	//this simple command updates the positions matrix. You need to do the same for color and indices matrices too
        mShader.uploadAttrib("vertexPosition_modelspace", positions);
        mShader.uploadAttrib("color", colors);
	mShader.uploadAttrib("vertexNormal_modelspace", normals);
	
        //This is a way to perform a simple rotation using a 4x4 rotation matrix represented by rmat
	//mvp stands for ModelViewProjection matrix
        Matrix4f mvp;
        mvp.setIdentity();
        mvp.topLeftCorner<3,3>() = Eigen::Matrix3f(Eigen::AngleAxisf(mRotation[0], Vector3f::UnitX()) *
                                                   Eigen::AngleAxisf(mRotation[1],  Vector3f::UnitY()) *
                                                   Eigen::AngleAxisf(mRotation[2], Vector3f::UnitZ())) * mZooming;
        mvp.topRightCorner<3,1>() = Eigen::Vector3f(mTranslation[0],mTranslation[1],mTranslation[2])* 0.25f;
       
        mShader.setUniform("MVP", mvp);

	// If enabled, does depth comparisons and update the depth buffer.
	// Avoid changing if you are unsure of what this means.
        glEnable(GL_DEPTH_TEST);
	
        /* Draw 12 triangles starting at index 0 of your indices matrix */
	/* Try changing the first input with GL_LINES, this will be useful in the assignment */
	/* Take a look at this link to better understand OpenGL primitives */
	/* https://www.khronos.org/opengl/wiki/Primitive */

	//12 triangles, each has three vertices
	mShader.drawArray(GL_TRIANGLES, 0, 12*3);

	//2 triangles, each has 3 lines, each line has 2 vertices
	//mShader.drawArray(GL_LINES, 12*3, 2*3*2);

        //mShader.drawIndexed(GL_TRIANGLES, 0, 12);
	//mShader.drawIndexed(GL_LINES, 12, 12);
        glDisable(GL_DEPTH_TEST);
    }

//Instantiation of the variables that can be acessed outside of this class to interact with the interface
//Need to be updated if a interface element is interacting with something that is inside the scope of MyGLCanvas
private:
    MatrixXf positions = MatrixXf(3, 48);
    MatrixXf normals = MatrixXf(3, 48);
    MatrixXf colors = MatrixXf(3, 48);
    WingedEdge *mWe;
    nanogui::GLShader mShader;
    Eigen::Vector3f mRotation;
    Eigen::Vector3f mTranslation;
    float mZooming = 0.25f;
};


class ExampleApplication : public nanogui::Screen {
public:
    ExampleApplication() : nanogui::Screen(Eigen::Vector2i(900, 600), "NanoGUI Cube and Menus", false) {
        using namespace nanogui;

	//OpenGL canvas demonstration

	//First, we need to create a window context in which we will render both the interface and OpenGL canvas
        Window *window = new Window(this, "GLCanvas Demo");
        window->setPosition(Vector2i(15, 15));
        window->setLayout(new GroupLayout());
	
	//OpenGL canvas initialization, we can control the background color and also its size
        mCanvas = new MyGLCanvas(window);
        mCanvas->setBackgroundColor({100, 100, 100, 255});
        mCanvas->setSize({400, 400});

	//This is how we add widgets, in this case, they are connected to the same window as the OpenGL canvas
        Widget *tools = new Widget(window);
        tools->setLayout(new BoxLayout(Orientation::Horizontal,
                                       Alignment::Middle, 0, 5));

	//then we start adding elements one by one as shown below
        Button *b0 = new Button(tools, "Random Color");
        b0->setCallback([this]() { mCanvas->setBackgroundColor(Vector4i(rand() % 256, rand() % 256, rand() % 256, 255)); });

        Button *b1 = new Button(tools, "Random Rotation");
        b1->setCallback([this]() { mCanvas->setRotation(nanogui::Vector3f((rand() % 100) / 100.0f, (rand() % 100) / 100.0f, (rand() % 100) / 100.0f)); });

        //widgets demonstration
        nanogui::GLShader mShader;

        // Control widgets
        Window *controlWindow = new Window(this, "Controls");
        controlWindow->setPosition(Vector2i(470, 15));
        controlWindow->setLayout(new GroupLayout());

        // Rotation panel
	Widget *panelRot = new Widget(controlWindow);
        panelRot->setLayout(new BoxLayout(Orientation::Vertical,
                                       Alignment::Middle, 6, 2));

        // Initiate rotation slider
	Slider *rotSlider_X = new Slider(panelRot);
        new Label(panelRot, "Rotation on X axis", "sans-bold");
        Slider *rotSlider_Y = new Slider(panelRot);
        new Label(panelRot, "Rotation on Y axis", "sans-bold");
	Slider *rotSlider_Z = new Slider(panelRot);
	new Label(panelRot, "Rotation on Z axis", "sans-bold");

        //Rotation along X axis
        rotSlider_X->setValue(0.5f);
        rotSlider_X->setFixedWidth(120);
        rotSlider_X->setCallback([&, rotSlider_Y, rotSlider_Z](float value) {
	    float radians_X = (value - 0.5f)*2*2*M_PI;
            float radians_Y = (rotSlider_Y->value() - 0.5f)*2*2*M_PI;
            float radians_Z = (rotSlider_Z->value() - 0.5f)*2*2*M_PI;          
	    mCanvas->setRotation(nanogui::Vector3f(radians_X, radians_Y, radians_Z));
        //     cout<<"radians_X = "<<radians_X<<" radians_Y = "<<radians_Y<<" radians_Z = "<<radians_Z<< endl;
        });

	//Rotation along Y axis
        rotSlider_Y->setValue(0.5f);
        rotSlider_Y->setFixedWidth(120);
        rotSlider_Y->setCallback([&, rotSlider_X, rotSlider_Z](float value) {
            float radians_X = (rotSlider_X->value() - 0.5f)*2*2*M_PI;
	    float radians_Y = (value - 0.5f)*2*2*M_PI;
            float radians_Z = (rotSlider_Z->value() - 0.5f)*2*2*M_PI;
	    mCanvas->setRotation(nanogui::Vector3f(radians_X, radians_Y, radians_Z));
        //     cout<<"radians_X = "<<radians_X<<" radians_Y = "<<radians_Y<<" radians_Z = "<<radians_Z<< endl;
        });

	//Rotation along Z axis
        rotSlider_Z->setValue(0.5f);
        rotSlider_Z->setFixedWidth(120);
        rotSlider_Z->setCallback([&, rotSlider_X, rotSlider_Y](float value) {
            float radians_X = (rotSlider_X->value() - 0.5f)*2*2*M_PI;
            float radians_Y = (rotSlider_Y->value() - 0.5f)*2*2*M_PI;
	    float radians_Z = (value - 0.5f)*2*2*M_PI;
	    mCanvas->setRotation(nanogui::Vector3f(radians_X, radians_Y, radians_Z));
        //     cout<<"radians_X = "<<radians_X<<" radians_Y = "<<radians_Y<<" radians_Z = "<<radians_Z<< endl;
        });

        // Translation panel
	Widget *panelTrans = new Widget(controlWindow);
        panelTrans->setLayout(new BoxLayout(Orientation::Vertical,
                                       Alignment::Middle, 6, 2));

        // Initiate translation slider
	Slider *tranSlider_X = new Slider(panelTrans);
        new Label(panelTrans, "Translation on X axis", "sans-bold");
        Slider *tranSlider_Y = new Slider(panelTrans);
        new Label(panelTrans, "Translation on Y axis", "sans-bold");
	Slider *tranSlider_Z = new Slider(panelTrans);
	new Label(panelTrans, "Translation on Z axis", "sans-bold");

        //Translation along X axis
        tranSlider_X->setValue(0.5f);
        tranSlider_X->setFixedWidth(120);
        tranSlider_X->setCallback([&, tranSlider_Y, tranSlider_Z](float value) {
            float trans_X = (value - 0.5f)*2*4;
            float trans_Y = (tranSlider_Y->value() - 0.5f)*2*4;
            float trans_Z = (tranSlider_Z->value() - 0.5f)*2*4;          
	    mCanvas->setTranslation(nanogui::Vector3f(trans_X, trans_Y, trans_Z));
        //     cout<<"trans_X = "<<trans_X<<" trans_Y = "<<trans_Y<<" trans_Z = "<<trans_Z<< endl;
        });

        //Translation along Y axis
        tranSlider_Y->setValue(0.5f);
        tranSlider_Y->setFixedWidth(120);
        tranSlider_Y->setCallback([&, tranSlider_X, tranSlider_Z](float value) {
            float trans_X = (tranSlider_X->value() - 0.5f)*2*4;
            float trans_Y = (value - 0.5f)*2*4;
            float trans_Z = (tranSlider_Z->value() - 0.5f)*2*4;          
	    mCanvas->setTranslation(nanogui::Vector3f(trans_X, trans_Y, trans_Z));
        //     cout<<"trans_X = "<<trans_X<<" trans_Y = "<<trans_Y<<" trans_Z = "<<trans_Z<< endl;
        });

        //Translation along Z axis
        tranSlider_Z->setValue(0.5f);
        tranSlider_Z->setFixedWidth(120);
        tranSlider_Z->setCallback([&, tranSlider_X, tranSlider_Y](float value) {
            float trans_X = (tranSlider_X->value() - 0.5f)*2*4; 
            float trans_Y = (tranSlider_Y->value() - 0.5f)*2*4;
            float trans_Z = (value - 0.5f)*2*4;        
	    mCanvas->setTranslation(nanogui::Vector3f(trans_X, trans_Y, trans_Z));
            cout<<"trans_X = "<<trans_X<<" trans_Y = "<<trans_Y<<" trans_Z = "<<trans_Z<< endl;
        });

        mCanvas->setTranslation(nanogui::Vector3f(0, 0, 0));

        // Zomming panel
	Widget *panelZoom = new Widget(controlWindow);
        panelZoom->setLayout(new BoxLayout(Orientation::Vertical,
                                       Alignment::Middle, 6, 2));

        // Initiate zooming slider
	Slider *zoom = new Slider(panelZoom);
        new Label(panelZoom, "Zooming", "sans-bold");
        zoom->setValue(0.5f);
        zoom->setFixedWidth(120);
        zoom->setCallback([&](float value) {
	    mCanvas->setZooming(value / 2.0f);
        });

	//Then, we can create another window and insert other widgets into it
	Window *anotherWindow = new Window(this, "Basic widgets");
        anotherWindow->setPosition(Vector2i(650, 15));
        anotherWindow->setLayout(new GroupLayout());

	// Demonstrates how a button called "New Mesh" can update the positions matrix.
	// This is just a demonstration, you actually need to bind mesh updates with the open file interface
	Button *button = new Button(anotherWindow, "New Mesh");
	button->setCallback([&] {
		//MatrixXf has dynamic size, so you can actually change its dimensions on the fly here
		//Make sure that the new mesh is not overblown by scaling it to a proper size and centering at origin
		//If you do not do that, the object may not appear at all, impacting the tests
		MatrixXf newPositions = MatrixXf(3, 8);
        	newPositions.col(5) << -1, -2, -1;
        	newPositions.col(6) <<  1, -1, -2;
        	newPositions.col(7) <<  1, -1,  2;
		newPositions.col(0) << -2,  1,  1;
        	newPositions.col(1) << -2,  1, -1;
        	newPositions.col(2) <<  1,  1, -1;
        	newPositions.col(3) <<  1,  1,  1;
        	newPositions.col(4) << -1, -2,  1;
        	newPositions.col(5) << -1, -2, -1;
        	newPositions.col(6) <<  1, -1, -2;
        	newPositions.col(7) <<  1, -1,  2;
		mCanvas->updateMeshPositions(newPositions);
	});
	button->setTooltip("Demonstrates how a button can update the positions matrix.");

	//Message dialog demonstration, it should be pretty straightforward
        new Label(anotherWindow, "Message dialog", "sans-bold");
        tools = new Widget(anotherWindow);
        tools->setLayout(new BoxLayout(Orientation::Horizontal,
                                       Alignment::Middle, 0, 6));
        Button *b = new Button(tools, "Info");
        b->setCallback([&] {
            auto dlg = new MessageDialog(this, MessageDialog::Type::Information, "Title", "This is an information message");
            dlg->setCallback([](int result) { cout << "Dialog result: " << result << endl; });
        });
        b = new Button(tools, "Warn");
        b->setCallback([&] {
            auto dlg = new MessageDialog(this, MessageDialog::Type::Warning, "Title", "This is a warning message");
            dlg->setCallback([](int result) { cout << "Dialog result: " << result << endl; });
        });
        b = new Button(tools, "Ask");
        b->setCallback([&] {
            auto dlg = new MessageDialog(this, MessageDialog::Type::Warning, "Title", "This is a question message", "Yes", "No", true);
            dlg->setCallback([](int result) { cout << "Dialog result: " << result << endl; });
        });

	//Here is how you can get the string that represents file paths both for opening and for saving.
	//you need to implement the rest of the parser logic.
        new Label(anotherWindow, "File dialog", "sans-bold");
        tools = new Widget(anotherWindow);
        tools->setLayout(new BoxLayout(Orientation::Horizontal,
                                       Alignment::Middle, 0, 6));
        b = new Button(tools, "Open");
        b->setCallback([&] {
            string fileName = file_dialog({ {"obj", "obj file"} }, false);
            mCanvas->loadObj(fileName);
        });
        b = new Button(tools, "Save");
        b->setCallback([&] {
            cout << "File dialog result: " << file_dialog(
                    { {"png", "Portable Network Graphics"}, {"txt", "Text file"} }, true) << endl;
        });

	//This is how to implement a combo box, which is important in A1
        new Label(anotherWindow, "Combo box", "sans-bold");
        ComboBox *combo = new ComboBox(anotherWindow, { "flat shaded", "smooth shaded in wireframe", "shaded with mesh edges"} );
        combo->setCallback([&](int value) {
            cout << "Combo box selected: " << value << endl;
        });	

        new Label(anotherWindow, "Check box", "sans-bold");
        CheckBox *cb = new CheckBox(anotherWindow, "Flag 1",
            [](bool state) { cout << "Check box 1 state: " << state << endl; }
        );
        cb->setChecked(true);
        cb = new CheckBox(anotherWindow, "Flag 2",
            [](bool state) { cout << "Check box 2 state: " << state << endl; }
        );
        new Label(anotherWindow, "Progress bar", "sans-bold");
        mProgress = new ProgressBar(anotherWindow);

        new Label(anotherWindow, "Slider and text box", "sans-bold");

        Widget *panel = new Widget(anotherWindow);
        panel->setLayout(new BoxLayout(Orientation::Horizontal,
                                       Alignment::Middle, 0, 20));

	//Fancy slider that has a callback function to update another interface element
        Slider *slider = new Slider(panel);
        slider->setValue(0.5f);
        slider->setFixedWidth(80);
        TextBox *textBox = new TextBox(panel);
        textBox->setFixedSize(Vector2i(60, 25));
        textBox->setValue("50");
        textBox->setUnits("%");
        slider->setCallback([textBox](float value) {
            textBox->setValue(std::to_string((int) (value * 100)));
        });
        slider->setFinalCallback([&](float value) {
            cout << "Final slider value: " << (int) (value * 100) << endl;
        });
        textBox->setFixedSize(Vector2i(60,25));
        textBox->setFontSize(20);
        textBox->setAlignment(TextBox::Alignment::Right);

	//Method to assemble the interface defined before it is called
        performLayout();
    }

    //This is how you capture mouse events in the screen. If you want to implement the arcball instead of using
    //sliders, then you need to map the right click drag motions to suitable rotation matrices
    virtual bool mouseMotionEvent(const Eigen::Vector2i &p, const Vector2i &rel, int button, int modifiers) override {
        if (button == GLFW_MOUSE_BUTTON_3 ) {
	    //Get right click drag mouse event, print x and y coordinates only if right button pressed
	    cout << p.x() << "     " << p.y() << "\n";
            return true;
        }
        return false;
    }

    virtual void drawContents() override {
        // ... put your rotation code here if you use dragging the mouse, updating either your model points, the mvp matrix or the V matrix, depending on the approach used
    }

    virtual void draw(NVGcontext *ctx) {
	/* Animate the scrollbar */
        mProgress->setValue(std::fmod((float) glfwGetTime() / 10, 1.0f));

        /* Draw the user interface */
        Screen::draw(ctx);
    }


private:
    nanogui::ProgressBar *mProgress;
    MyGLCanvas *mCanvas;
};

int main(int /* argc */, char ** /* argv */) {
    try {
        nanogui::init();

            /* scoped variables */ {
            nanogui::ref<ExampleApplication> app = new ExampleApplication();
            app->drawAll();
            app->setVisible(true);
            nanogui::mainloop();
        }

        nanogui::shutdown();
    } catch (const std::runtime_error &e) {
        std::string error_msg = std::string("Caught a fatal error: ") + std::string(e.what());
        #if defined(_WIN32)
            MessageBoxA(nullptr, error_msg.c_str(), NULL, MB_ICONERROR | MB_OK);
        #else
            std::cerr << error_msg << endl;
        #endif
        return -1;
    }

    return 0;
}
