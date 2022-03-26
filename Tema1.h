#pragma once

#include "components/simple_scene.h"


namespace m1
{
    class Tema1 : public gfxc::SimpleScene
    {
    public:

        Tema1();
        ~Tema1();

        void Init() override;

        struct Obstacole {
            float x;
            float y;
            float w;
            float h;
        };

        struct Enemies {
            float x;
            float y;
            float viteza;
            bool exista;
        };

        struct ViewportSpace
        {
            ViewportSpace() : x(0), y(0), width(1), height(1) {}
            ViewportSpace(int x, int y, int width, int height)
                : x(x), y(y), width(width), height(height) {}
            int x;
            int y;
            int width;
            int height;
        };

        struct LogicSpace
        {
            LogicSpace() : x(0), y(0), width(1), height(1) {}
            LogicSpace(float x, float y, float width, float height)
                : x(x), y(y), width(width), height(height) {}
            float x;
            float y;
            float width;
            float height;
        };

    private:
        void FrameStart() override;
        void Update(float deltaTimeSeconds) override;
        void FrameEnd() override;

        void OnInputUpdate(float deltaTime, int mods) override;
        void OnKeyPress(int key, int mods) override;
        void OnKeyRelease(int key, int mods) override;
        void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
        void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
        void OnWindowResize(int width, int height) override;

        bool CheckCollision(float x, float y, float one, float two);
        bool CheckCollision_rect(float x, float y, float w, float h);
        void init_obstacole_array();

        void DrawScene(glm::mat3 visMatrix);


        // Sets the logic space and view space
        // logicSpace: { x, y, width, height }
        // viewSpace: { x, y, width, height }
        glm::mat3 VisualizationTransf2DUnif(const LogicSpace& logicSpace, const ViewportSpace& viewSpace);

        void SetViewportArea(const ViewportSpace& viewSpace, glm::vec3 colorColor = glm::vec3(0), bool clear = true);




    protected:

        float length, length1, length2, length3, length4, scale_proiectil;
        ViewportSpace viewSpace;
        LogicSpace logicSpace;
        glm::mat3 visMatrix;
        float map_width, map_height;
        float left_x, right_x, bottom_y, top_y;

        Obstacole obstac[11];
        int nr_obstacole;


        float cx, cy;
        glm::mat3 modelMatrix;
        float translateX, translateY;
        float scaleX, scaleY;
        float angularStep;

        // TODO(student): If you need any other class variables, define them here.


        float circle_radius, eye_radius, x, y, h;
        int mousemoveX, mousemoveY;
        float translate_X_Player, translate_Y_Player, translate_X_Proiectil, translate_Y_Proiectil;
        float angular_eye, angular_proiectil;

        int move_proiectil;
        float time_to_fire;
        float fire_rate;


        float proiectil_distance, max_distance;
        bool proiectil_exists;

        float translate_X_Enemy, translate_Y_Enemy;
        float angular_enemy, enemy_speed;
        float enemy_eye_leng, enemy_leng;

        Enemies inamici[20];
        unsigned short en_id;
        float time_enemy;
        unsigned short nr_enemies;

        unsigned short Score;
        int HP;
    };
}   // namespace m1
