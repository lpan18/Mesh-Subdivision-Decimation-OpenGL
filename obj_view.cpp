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
        
        // After binding the shader to the current context we can send data to opengl that will be handled
        // by the vertex shader and then by the fragment shader, in that order.
        // if you want to know more about modern opengl pipeline take a look at this link
        // https://www.khronos.org/opengl/wiki/Rendering_Pipeline_Overview
        mShader.bind();

        // ViewMatrixID
        // change your rotation to work on the camera instead of rotating the entire world with the MVP matrix
        Matrix4f V;
        V.setIdentity();
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
        delete mWe;
    }

    void loadObj(string fileName) {
        mWe = new WingedEdge(fileName);
        positions = mWe->getPositions();
        normals = mWe->getNormals(positions);
        smoothNormals = mWe->getSmoothNormals(normals);
        colors = mWe->getColors();
    }

    void writeObj(string fileName) {
        if (mWe != NULL) {
            mWe->writeObj(fileName);
        } else {
            cout << "No object in scene" << endl;
        }
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

    void setShadingMode(float fShadingMode) {
        mShadingMode = fShadingMode;
    }

    //OpenGL calls this method constantly to update the screen.
    virtual void drawGL() override {
        using namespace nanogui;
        if (mWe == NULL) return;
        
	    //refer to the previous explanation of mShader.bind();
        mShader.bind();

        MatrixXf shadingNormal = (mShadingMode == 1 || mShadingMode == 4)  ? smoothNormals : normals;
	    //this simple command updates the positions matrix. You need to do the same for color and indices matrices too
        mShader.uploadAttrib("vertexPosition_modelspace", positions);
        mShader.uploadAttrib("color", colors);
	    mShader.uploadAttrib("vertexNormal_modelspace", shadingNormal);
	
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
	    // mShader.drawArray(GL_TRIANGLES, 0, positions.cols());
        if (mShadingMode != 2) {
            mShader.drawArray(GL_TRIANGLES, 0, positions.cols() / 3);
        }
        if (mShadingMode != 0 && mShadingMode != 1) {
            mShader.drawArray(GL_LINES, positions.cols() / 3, positions.cols());
        }
        // mShader.drawArray(GL_LINES, positions.cols() / 3, positions.cols());

	    //2 triangles, each has 3 lines, each line has 2 vertices
	    //mShader.drawArray(GL_LINES, 12*3, 2*3*2);

        //mShader.drawIndexed(GL_TRIANGLES, 0, 12);
	    //mShader.drawIndexed(GL_LINES, 12, 12);
        glDisable(GL_DEPTH_TEST);
    }

//Instantiation of the variables that can be acessed outside of this class to interact with the interface
//Need to be updated if a interface element is interacting with something that is inside the scope of MyGLCanvas
private:
    WingedEdge *mWe;
    MatrixXf positions;
    MatrixXf normals;
    MatrixXf smoothNormals;
    MatrixXf colors;
    nanogui::GLShader mShader;
    Eigen::Vector3f mRotation;
    Eigen::Vector3f mTranslation;
    float mZooming = 0.5f;
    int mShadingMode = 0;
};


class ExampleApplication : public nanogui::Screen {
public:
    ExampleApplication() : nanogui::Screen(Eigen::Vector2i(1100, 600), "NanoGUI Assignment1", false) {
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

    	//Then, we can create another window and insert other widgets into it
	    Window *anotherWindow = new Window(this, "Basic widgets");
        anotherWindow->setPosition(Vector2i(500, 15));
        anotherWindow->setLayout(new GroupLayout());

	    //Here is how you can get the string that represents file paths both for opening and for saving.
        new Label(anotherWindow, "File dialog", "sans-bold");
        Button *openBtn  = new Button(anotherWindow, "Open");
        openBtn->setCallback([&] {
            string fileName = file_dialog({ {"obj", "obj file"} }, false);
            mCanvas->loadObj(fileName);
        });
        Button *saveBtn = new Button(anotherWindow, "Save");
        saveBtn->setCallback([&] {
            string fileName = file_dialog({ {"obj", "obj file"} }, true);
            mCanvas->writeObj(fileName);
        });

        // Rotation panel
        new Label(anotherWindow, "Rotation", "sans-bold");
	    Widget *panelRot = new Widget(anotherWindow);
        panelRot->setLayout(new BoxLayout(Orientation::Horizontal,
                                       Alignment::Middle, 0, 2));

        // Initiate rotation slider
	    Slider *rotSlider_X = new Slider(panelRot);
	    new Label(panelRot, "X");
        Slider *rotSlider_Y = new Slider(panelRot);
	    new Label(panelRot, "Y");
	    Slider *rotSlider_Z = new Slider(panelRot);
	    new Label(panelRot, "Z");

        //Rotation along X axis
        rotSlider_X->setValue(0.5f);
        rotSlider_X->setFixedWidth(80);
        rotSlider_X->setCallback([&, rotSlider_Y, rotSlider_Z](float value) {
	        float radians_X = (value - 0.5f)*2*2*M_PI;
            float radians_Y = (rotSlider_Y->value() - 0.5f)*2*2*M_PI;
            float radians_Z = (rotSlider_Z->value() - 0.5f)*2*2*M_PI;          
	        mCanvas->setRotation(nanogui::Vector3f(radians_X, radians_Y, radians_Z));
        });

	    //Rotation along Y axis
        rotSlider_Y->setValue(0.5f);
        rotSlider_Y->setFixedWidth(80);
        rotSlider_Y->setCallback([&, rotSlider_X, rotSlider_Z](float value) {
            float radians_X = (rotSlider_X->value() - 0.5f)*2*2*M_PI;
	        float radians_Y = (value - 0.5f)*2*2*M_PI;
            float radians_Z = (rotSlider_Z->value() - 0.5f)*2*2*M_PI;
	        mCanvas->setRotation(nanogui::Vector3f(radians_X, radians_Y, radians_Z));
        });

	    //Rotation along Z axis
        rotSlider_Z->setValue(0.5f);
        rotSlider_Z->setFixedWidth(80);
        rotSlider_Z->setCallback([&, rotSlider_X, rotSlider_Y](float value) {
            float radians_X = (rotSlider_X->value() - 0.5f)*2*2*M_PI;
            float radians_Y = (rotSlider_Y->value() - 0.5f)*2*2*M_PI;
	        float radians_Z = (value - 0.5f)*2*2*M_PI;
	        mCanvas->setRotation(nanogui::Vector3f(radians_X, radians_Y, radians_Z));
        });

        // Translation panel
        new Label(anotherWindow, "Translation", "sans-bold");
	    Widget *panelTrans = new Widget(anotherWindow);
        panelTrans->setLayout(new BoxLayout(Orientation::Horizontal,
                                       Alignment::Middle, 0, 2));

        // Initiate translation slider
	    Slider *tranSlider_X = new Slider(panelTrans);
	    new Label(panelTrans, "X");
        Slider *tranSlider_Y = new Slider(panelTrans);
	    new Label(panelTrans, "Y");
	    Slider *tranSlider_Z = new Slider(panelTrans);
	    new Label(panelTrans, "Z");

        //Translation along X axis
        tranSlider_X->setValue(0.5f);
        tranSlider_X->setFixedWidth(80);
        tranSlider_X->setCallback([&, tranSlider_Y, tranSlider_Z](float value) {
            float trans_X = (value - 0.5f)*2*4;
            float trans_Y = (tranSlider_Y->value() - 0.5f)*2*4;
            float trans_Z = (tranSlider_Z->value() - 0.5f)*2*4;          
	        mCanvas->setTranslation(nanogui::Vector3f(trans_X, trans_Y, trans_Z));
        });

        //Translation along Y axis
        tranSlider_Y->setValue(0.5f);
        tranSlider_Y->setFixedWidth(80);
        tranSlider_Y->setCallback([&, tranSlider_X, tranSlider_Z](float value) {
            float trans_X = (tranSlider_X->value() - 0.5f)*2*4;
            float trans_Y = (value - 0.5f)*2*4;
            float trans_Z = (tranSlider_Z->value() - 0.5f)*2*4;          
	        mCanvas->setTranslation(nanogui::Vector3f(trans_X, trans_Y, trans_Z));
        });

        //Translation along Z axis
        tranSlider_Z->setValue(0.5f);
        tranSlider_Z->setFixedWidth(80);
        tranSlider_Z->setCallback([&, tranSlider_X, tranSlider_Y](float value) {
            float trans_X = (tranSlider_X->value() - 0.5f)*2*4; 
            float trans_Y = (tranSlider_Y->value() - 0.5f)*2*4;
            float trans_Z = (value - 0.5f)*2*4;        
	        mCanvas->setTranslation(nanogui::Vector3f(trans_X, trans_Y, trans_Z));
        });

        mCanvas->setTranslation(nanogui::Vector3f(0, 0, 0));

        // Zomming panel
	    new Label(anotherWindow, "Zoom", "sans-bold");
	    Widget *panelZoom = new Widget(anotherWindow);
        panelZoom->setLayout(new BoxLayout(Orientation::Horizontal,
                                       Alignment::Middle, 0, 2));

        // Initiate zooming slider
        Slider *zoom = new Slider(panelZoom);
        zoom->setValue(0.5f);
        zoom->setFixedWidth(150);
        // zoom->setCallback([&](float value) {
	    //     mCanvas->setZooming(value);
        // });

        TextBox *zoomTxt = new TextBox(panelZoom);
        zoomTxt->setFixedSize(Vector2i(60, 25));
        zoomTxt->setValue("50");
        zoomTxt->setUnits("%");
        zoom->setCallback([&, zoomTxt](float value) {
	        mCanvas->setZooming(value);
            zoomTxt->setValue(std::to_string((int) (value * 100)));
        });
        zoomTxt->setFixedSize(Vector2i(60,25));
        zoomTxt->setFontSize(20);
        zoomTxt->setAlignment(TextBox::Alignment::Right);


    	//This is how to implement a combo box, which is important in A1
        new Label(anotherWindow, "Shading Mode", "sans-bold");
        Widget *panelCombo = new Widget(anotherWindow);
        panelCombo->setLayout(new BoxLayout(Orientation::Horizontal,
                                       Alignment::Middle, 0, 2));

        ComboBox *combo = new ComboBox(anotherWindow, { "Flat", "Smooth", "Wireframe", "Flat+Wireframe", "Smooth+Wireframe"} );
        combo->setCallback([&](int value) {
            mCanvas->setShadingMode(value);
        });	

        new Label(anotherWindow, "Progress bar", "sans-bold");
        mProgress = new ProgressBar(anotherWindow);

        Button *quitBtn = new Button(anotherWindow, "Quit");
        quitBtn->setCallback([&] {
            auto dlg = new MessageDialog(this, MessageDialog::Type::Warning, "Quit", "Are you sure to shut down?", "No", "Yes", true);
            dlg->setCallback([](int result) { 
                if(result == 1){
                    nanogui::shutdown();
                }});
        });
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
