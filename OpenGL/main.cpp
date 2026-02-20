#include <iostream> // إدخال وإخراج
#define GLEW_STATIC
#include <GL/glew.h> // مكتبة GLEW
#include <GLFW/glfw3.h> // مكتبة النوافذ
#include <cstring> // لننسخ مصفوفات

const unsigned int SCR_WIDTH = 800; // عرض الشاشة
const unsigned int SCR_HEIGHT = 600; // طول الشاشة

// ==== Vertex Shader ====
const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n" // موقع النقطة
"layout (location = 1) in vec3 aColor;\n" // لون النقطة
"out vec3 ourColor;\n" // منبعت اللون للمرحلة الجاية
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos, 1.0);\n" // منحط الموقع
"   ourColor = aColor;\n" // منمرر اللون
"}\0";

// ==== Fragment Shader ====
const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n" // اللون النهائي
"in vec3 ourColor;\n" // جاي من فوق
"uniform float uAlpha;\n" // شفافية
"void main()\n"
"{\n"
"   FragColor = vec4(ourColor, uAlpha);\n" // منرسم مع الشفافية
"}\n\0";

// ==== متغيرات تحكم ====
float redOffsetX = 0.0f; // إزاحة الأحمر
float blueOffsetX = 0.0f; // إزاحة الأزرق
bool redGreen = false; // الأحمر يقلب أخضر؟
bool bluePink = false; // الأزرق يقلب زهري؟

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height); // نعدل المقاس
}

void processInput(GLFWwindow* window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true); // خروج

    if(glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // خطوط بس
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // رسم عادي

    bluePink = glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS; // W يزهر الأزرق
    redGreen = glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS; // D يخضر الأحمر

    if(glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        blueOffsetX += 0.01f; // الأزرق يمين

    if(glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        redOffsetX -= 0.01f; // الأحمر يسار
}

int main()
{
    glfwInit(); // نشغل GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3); // نسخة 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH,SCR_HEIGHT,"Interactive Triangles",NULL,NULL);
    if(!window){ glfwTerminate(); return -1; } // إذا ما زبطت نطلع
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    glewExperimental = GL_TRUE;
    if(glewInit()!=GLEW_OK) return -1; // إذا GLEW فشل نطلع

    glEnable(GL_DEPTH_TEST); // تفعيل العمق
    glEnable(GL_BLEND); // تفعيل الشفافية
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // طريقة الدمج

    // ==== بناء الشيدر ====
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader,1,&vertexShaderSource,NULL);
    glCompileShader(vertexShader); // ترجمة

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader,1,&fragmentShaderSource,NULL);
    glCompileShader(fragmentShader);

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram); // ربط
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // ==== بيانات المثلثات ====
    float vertices[] = {
        // الأحمر
        -0.5f,-0.5f,0.0f, 1.0f,0.0f,0.0f,
         0.5f,-0.5f,0.0f, 1.0f,0.0f,0.0f,
         0.0f, 0.5f,0.0f, 1.0f,0.0f,0.0f,
        // الأزرق
        -0.2f,-0.2f,0.5f, 0.0f,0.0f,1.0f,
         0.8f,-0.2f,0.5f, 0.0f,0.0f,1.0f,
         0.3f, 0.8f,0.5f, 0.0f,0.0f,1.0f
    };

    unsigned int VBO, VAO;
    glGenVertexArrays(1,&VAO); // إنشاء VAO
    glGenBuffers(1,&VBO); // إنشاء VBO

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER,VBO);
    glBufferData(GL_ARRAY_BUFFER,sizeof(vertices),vertices,GL_DYNAMIC_DRAW); // تحميل البيانات

    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,6*sizeof(float),(void*)0); // موقع
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,6*sizeof(float),(void*)(3*sizeof(float))); // لون
    glEnableVertexAttribArray(1);

    while(!glfwWindowShouldClose(window))
    {
        processInput(window); // نقرأ الأزرار

        glClearColor(0.1f,0.1f,0.1f,1.0f); // خلفية غامقة
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);

        int alphaLoc = glGetUniformLocation(shaderProgram,"uAlpha"); // مكان الألفا

        // ==== الأحمر ====
        float redVertices[18];
        memcpy(redVertices, vertices, sizeof(redVertices)); // ننسخ الأصل
        for(int i=0;i<3;i++)
            redVertices[i*6+0] += redOffsetX; // نحركه
        if(redGreen)
            for(int i=0;i<3;i++){ // إذا D
                redVertices[i*6+3]=0.0f;
                redVertices[i*6+4]=1.0f;
                redVertices[i*6+5]=0.0f;
            }
        glBufferSubData(GL_ARRAY_BUFFER,0,sizeof(redVertices),redVertices);
        glUniform1f(alphaLoc,0.6f); // شفاف شوي
        glDrawArrays(GL_TRIANGLES,0,3);

        // ==== الأزرق ====
        float blueVertices[18];
        memcpy(blueVertices,&vertices[18],sizeof(blueVertices)); // ننسخ الأزرق
        for(int i=0;i<3;i++)
            blueVertices[i*6+0] += blueOffsetX; // نحركه
        if(bluePink)
            for(int i=0;i<3;i++){ // إذا W
                blueVertices[i*6+3]=1.0f;
                blueVertices[i*6+4]=0.4f;
                blueVertices[i*6+5]=0.7f;
            }
        glBufferSubData(GL_ARRAY_BUFFER,3*6*sizeof(float),sizeof(blueVertices),blueVertices);
        glUniform1f(alphaLoc,1.0f); // كامل
        glDrawArrays(GL_TRIANGLES,3,3);

        glfwSwapBuffers(window); // تبديل الفريم
        glfwPollEvents(); // تحديث الأحداث
    }

    glDeleteVertexArrays(1,&VAO); // تنظيف
    glDeleteBuffers(1,&VBO);
    glDeleteProgram(shaderProgram);
    glfwTerminate(); // سكّر كل شي
    return 0;
}
