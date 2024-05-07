#include <GL/glut.h>
#include <cmath>
#include <cstdlib>
#include <random>
#include <GL/freeglut.h>
#include <glm/glm.hpp>

int n;

GLfloat camera_position[3] = {2, 1, 2};
GLfloat prism_center[3] = {0, 0, 0};
GLfloat prism_center2[3] = {0, 0, -1};
GLfloat center[3] = {0, 0, 0};
float rotation_angle = 0;
float rotation_angle_cam = 0;
int toggle_val = 0;
float gl_val_x = 0;
float gl_val_y = 0;
float gl_val_z = 0;

typedef struct Vector3f
{
    float x;
    float y;
    float z;
} Vector3f;

Vector3f init_Vector3f(float x, float y, float z)
{
    Vector3f vec;
    vec.x = x;
    vec.y = y;
    vec.z = z;
    return vec;
}

typedef struct store_sides
{
    Vector3f top_left;
    Vector3f top_right;
    Vector3f bottom_left;
    Vector3f bottom_right;
} store_sides;

store_sides init_store_sides(float height, float bottom_left_x, float bottom_left_y, float bottom_right_x, float bottom_right_y)
{

    store_sides s;
    s.bottom_left = init_Vector3f(center[0] + bottom_left_x, center[1] + bottom_left_y, center[2]);
    s.top_left = init_Vector3f(center[0] + bottom_left_x, center[1] + bottom_left_y, center[2] + height);
    s.top_right = init_Vector3f(center[0] + bottom_right_x, center[1] + bottom_right_y, center[2] + height);
    s.bottom_right = init_Vector3f(center[0] + bottom_right_x, center[1] + bottom_right_y, center[2]);
    return s;
}

typedef struct store_sides_tri
{
    Vector3f top_center;
   
    Vector3f bottom_left;
    Vector3f bottom_right;
} store_sides_tri;

store_sides_tri init_store_sides_tri(float height, float bottom_left_x, float bottom_left_y, float bottom_right_x, float bottom_right_y)
{

    store_sides_tri s;
    s.bottom_left = init_Vector3f(bottom_left_x,bottom_left_y, center[2]);
    s.top_center = init_Vector3f(center[0] , center[1] , center[2] + height);
    s.bottom_right = init_Vector3f(bottom_right_x,bottom_right_y, center[2]);
    return s;
}

Vector3f cross_product(Vector3f c1, Vector3f c2)
{
    Vector3f ans;
    ans.x = c1.y * c2.z - c1.z * c2.y;
    ans.y = c1.z * c2.x - c1.x * c2.z;
    ans.z = c1.x * c2.y - c1.y * c2.x;
    return ans;
}

void keyboard_input(unsigned char key, int x, int y)
{
    float camera_speed = 0.5;
    // y axis is up and down, z axis is front and back, x axis is left and right

    float magnitude = (camera_position[0] * camera_position[0] + camera_position[1] * camera_position[1] + camera_position[2] * camera_position[2]);
    magnitude = sqrt(magnitude);
    if (key == 'w')
    {
        camera_position[0] -= camera_speed * camera_position[0] / magnitude;
        camera_position[1] -= camera_speed * camera_position[1] / magnitude;
        camera_position[2] -= camera_speed * camera_position[2] / magnitude;
    }
    else if (key == 's')
    {

        camera_position[0] += camera_speed * camera_position[0] / magnitude;
        camera_position[1] += camera_speed * camera_position[1] / magnitude;
        camera_position[2] += camera_speed * camera_position[2] / magnitude;
    }
    else if (key == 'a')
    {

        // Vector3f cam = init_Vector3f(prism_center[0] - camera_position[0], prism_center[1] - camera_position[1], prism_center[2] - camera_position[2]);
        // float mag = (cam.x * cam.x + cam.y * cam.y + cam.z * cam.z);
        // mag = sqrt(mag);
        // cam.x = cam.x / mag;
        // cam.y = cam.y / mag;
        // cam.z = cam.z / mag;
        // Vector3f camvec = cross_product(cam, init_Vector3f(0, 1, 0));

        // camera_position[0] -= camvec.x * camera_speed / magnitude;
        // camera_position[1] -= camvec.y * camera_speed / magnitude;
        // camera_position[2] -= camvec.z * camera_speed / magnitude;

        // toggle_val = 0;
       camera_position[0] -= camera_speed;
    }
    else if (key == 'd')
    {

        // Vector3f cam = init_Vector3f(prism_center[0] - camera_position[0], prism_center[1] - camera_position[1], prism_center[2] - camera_position[2]);
        // float mag = (cam.x * cam.x + cam.y * cam.y + cam.z * cam.z);
        // mag = sqrt(mag);
        // cam.x = cam.x / mag;
        // cam.y = cam.y / mag;
        // cam.z = cam.z / mag;
        //camera_position[0] += camera_speed;
        //  Vector3f camvec = cross_product(cam, init_Vector3f(0, 1, 0));

        // camera_position[0] += camvec.x * camera_speed / magnitude;
        // camera_position[1] += camvec.y * camera_speed / magnitude;
        // camera_position[2] += camvec.z * camera_speed / magnitude;

         camera_position[0] += camera_speed;
        // prism_center[0] -= camera_speed;
        // toggle_val = 0;
    }
    else if (key == 'q')
    {
        camera_position[1] -= camera_speed;
        // prism_center[1] -= camera_speed;
        // prism_center[0] -= camera_speed;
        // prism_center[2] -= camera_speed;
        // camera_position[0] -= camera_speed;
        // camera_position[1] += camera_speed;
        // camera_position[2] -= camera_speed;


    }
    else if (key == 'e')
    {
        // camera_position[1] -= camera_speed;
        //  prism_center[1] += camera_speed;
        camera_position[1] += camera_speed;
        // prism_center[1] += camera_speed;
        // prism_center[0] += camera_speed;
        // prism_center[2] += camera_speed;
        // toggle_val = 0;
    }
    else if (key == '1')
    {
        // center[1] += camera_speed;
        gl_val_y += 0.1;
        // toggle_val = 0;
    }
    else if (key == '2')
    {
        gl_val_y -= 0.1;
        // center[1] -= camera_speed;
        // toggle_val = 0;
    }
    else if (key == '3')
    {

        // center[0] -= camera_speed;
        gl_val_x -= 0.1;
        // toggle_val = 0;
    }
    else if (key == '4')
    {
        gl_val_x += 0.1;
        // center[0] += camera_speed;
        // toggle_val = 0;
    }
    else if (key == '5')
    {
        gl_val_z += 0.1;
        // center[2] += camera_speed;
        // toggle_val = 0;
    }
    else if (key == '6')
    {
        gl_val_z -= 0.1;
        // center[2] -= camera_speed;
        // toggle_val = 0;
    }
    else if (key == 'r')
    {
        rotation_angle += 1;
        //  toggle_val = 0;
    }
    else if (key == 't')
    {
        if (toggle_val == 0)
        {
            toggle_val = 1;
        }
        else
        {
            toggle_val = 0;
        }
    }
    glutPostRedisplay();
}

void draw_prism(float radius, float height, int n)
{

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> distrib(0.0f, 1.0f);
    // Drawing the bottom face

    GLfloat bottom_color[3];
    bottom_color[0] = 0.85358;
    bottom_color[1] = 0.629583;
    bottom_color[2] = 0.28572;
    glColor3fv(bottom_color);

    glBegin(GL_POLYGON);
    float angle_inc = (2 * M_PI) / n;
    // angle_inc = (M_PI * (n-2))/ ( n);

    for (int i = 0; i < n; i++)
    {
        float theta = angle_inc * i;
        float x = radius * cosf(theta);
        float y = radius * sinf(theta);
        glVertex3f(center[0] + x, center[1] + y, center[2]);
    }
    glEnd();

    // Vector3f color_array[] = {
    //     init_Vector3f(1.0f, 0.5f, 0.5f),
    //     init_Vector3f(1.0f, 0.75f, 0.5f),
    //     init_Vector3f(1.0f, 1.0f, 0.5f),
    //     init_Vector3f(0.5f, 1.0f, 0.5f),
    //     init_Vector3f(0.5f, 0.5f, 1.0f),
    //     init_Vector3f(0.75f, 0.5f, 1.0f)};

    // Drawing side faces

    GLfloat color[3];
    int rand_val = distrib(gen);
    // color[0] = color_array[ind].x;
    // color[1] = color_array[ind].y;
    // color[2] = color_array[ind].z;

    for (int i = 0; i < n; i++)
    {

        // int rand_color = distrib(gen);
        // int ind = rand_color % 6;

        // color[0] = color_array[ind].x;
        // color[1] = color_array[ind].y;
        // color[2] = color_array[ind].z;
        color[0] = ((2 * i + 2) / (float)(2 * n + 2));
        color[1] = (2 * i + 6) / (float)(2 * n + 6);
        color[2] = (2 * i + 8) / (float)(2 * n + 8);

        glColor3fv(color);

        glBegin(GL_QUADS);
        float t1 = angle_inc * i;
        float bottom_left_x = radius * cos(t1);
        float bottom_left_y = radius * sin(t1);

        float t2 = angle_inc * ((i + 1) % n);
        float bottom_right_x = radius * cos(t2);
        float bottom_right_y = radius * sin(t2);

        store_sides sides = init_store_sides(height, bottom_left_x, bottom_left_y, bottom_right_x, bottom_right_y);

        GLfloat bottom_left[3];
        bottom_left[0] = sides.bottom_left.x;
        bottom_left[1] = sides.bottom_left.y;
        bottom_left[2] = sides.bottom_left.z;

        GLfloat top_left[3];
        top_left[0] = sides.top_left.x;
        top_left[1] = sides.top_left.y;
        top_left[2] = sides.top_left.z;

        GLfloat bottom_right[3];
        bottom_right[0] = sides.bottom_right.x;
        bottom_right[1] = sides.bottom_right.y;
        bottom_right[2] = sides.bottom_right.z;

        GLfloat top_right[3];
        top_right[0] = sides.top_right.x;
        top_right[1] = sides.top_right.y;
        top_right[2] = sides.top_right.z;

        // Bottom-left vertex
        glVertex3fv(bottom_left);
        // Top-left vertex
        glVertex3fv(top_left);
        // Top-right vertex
        glVertex3fv(top_right);
        // Bottom-right vertex
        glVertex3fv(bottom_right);

        glEnd();
    }

    // Drawing the top face

    GLfloat top_color[3];
    top_color[0] = 0.388482;
    top_color[1] = 0.285729;
    top_color[2] = 0.358749;
    glColor3fv(top_color);
    glBegin(GL_POLYGON);
    for (int i = 0; i < n; i++)
    {
        float theta = angle_inc * i;
        float x = radius * cosf(theta);
        float y = radius * sinf(theta);
        glVertex3f(center[0] + x, center[1] + y, center[2] + height);
    }
    glEnd();
}

void draw_pyramid(float radius, float height, int n)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> distrib(0.0f, 1.0f);

    float angle_inc = (2 * M_PI) / n;

    glBegin(GL_TRIANGLES);
    for (int i = 0; i < n; i++)
    {
        GLfloat tri_color[3];
        tri_color[0] = ((2 * i + 2) / (float)(2 * n + 2));
        tri_color[1] = (2 * i + 6) / (float)(2 * n + 6);
        tri_color[2] = (2 * i + 8) / (float)(2 * n + 8);
        glColor3fv(tri_color);

        float t1 = angle_inc * i;
        float bottom_left_x = radius * cosf(t1) + center[0];
        float bottom_left_y = radius * sinf(t1) + center[1];

        float t2 = angle_inc * (i + 1);
        float bottom_right_x = radius * cosf(t2) + center[0];
        float bottom_right_y = radius * sinf(t2) + center[1];

        store_sides_tri sides = init_store_sides_tri(height,bottom_left_x,bottom_left_y,bottom_right_x,bottom_right_y);
        
        GLfloat bottom_left[3];
        bottom_left[0] = sides.bottom_left.x;
        bottom_left[1] = sides.bottom_left.y;
        bottom_left[2] = sides.bottom_left.z;

        GLfloat top_center[3];
        top_center[0] = sides.top_center.x;
        top_center[1] = sides.top_center.y;
        top_center[2] = sides.top_center.z;

        GLfloat bottom_right[3];
        bottom_right[0] = sides.bottom_right.x;
        bottom_right[1] = sides.bottom_right.y;
        bottom_right[2] = sides.bottom_right.z;


        glVertex3fv(top_center);
        glVertex3fv(bottom_left);
        glVertex3fv(bottom_right);
    }
    glEnd();

    // Drawing the base
    GLfloat bottom_color[3];
    bottom_color[0] = 0.85358;
    bottom_color[1] = 0.629583;
    bottom_color[2] = 0.28572;
    glColor3fv(bottom_color);

    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(center[0], center[1], center[2]);
    for (int i = 0; i < n + 1; i++)
    {
        float theta = angle_inc * i;
        float x = radius * cosf(theta) + center[0];
        float y = radius * sinf(theta) + center[1];
        glVertex3f(x, y, center[2]);
    }
    glEnd();
}

void render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(gl_val_x, gl_val_y, gl_val_z);
    gluLookAt(camera_position[0], camera_position[1], camera_position[2], prism_center[0], prism_center[1], prism_center[2], 0, 1, 0);
    glRotatef(rotation_angle, 0, 0, 1);
   // glRotatef(rotation_angle_cam, 0, 0, 1);
    float radius = 0.5;
    float height = 1;
    if (toggle_val == 0)
        draw_prism(radius, height, n);
    else
        draw_pyramid(radius, height, n);

   
    glutSwapBuffers();
}

int main(int argc, char **argv)
{

    n = std::atoi(argv[1]);

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Prism");

    glEnable(GL_DEPTH_TEST);

    glViewport(0, 0, 800, 600);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(90, 800.0 / 600.0, 0.1, 50);
    glMatrixMode(GL_MODELVIEW);
    glutDisplayFunc(render);
    glutKeyboardFunc(keyboard_input);
    glutMainLoop();

    return 0;
}
