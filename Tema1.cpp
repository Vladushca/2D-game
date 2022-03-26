#include "lab_m1/Tema1/Tema1.h"

#include <vector>
#include <iostream>

#include "lab_m1/Tema1/transform2D.h"
#include "lab_m1/Tema1/object2D.h"

using namespace std;
using namespace m1;


/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */


Tema1::Tema1()
{
}


Tema1::~Tema1()
{
}


void Tema1::Init()
{
    glm::ivec2 resolution = window->GetResolution();
    auto camera = GetSceneCamera();
    camera->SetOrthographic(0, (float)resolution.x, 0, (float)resolution.y, 0.01f, 400);
    camera->SetPosition(glm::vec3(0, 0, 50));
    camera->SetRotation(glm::vec3(0, 0, 0));
    camera->Update();
    GetCameraInput()->SetActive(false);

    glm::vec3 corner = glm::vec3(0.001, 0.001, 0);

    length = 0.5f;
    length1 = 1;
    length2 = 2;
    length3 = 4;


    /////////////////////////////////////////////////////////////////////////////////////////

    logicSpace.x = 0;       // logic x
    logicSpace.y = 0;       // logic y
    logicSpace.width = 4;   // logic width
    logicSpace.height = 4;  // logic height

    map_width = 11;
    map_height = 8;

    circle_radius = 0.2f;
    eye_radius = 0.08f;

    enemy_eye_leng = 0.15f;
    enemy_leng = 0.4f;

    glm::vec3 left_russian = glm::vec3(-enemy_eye_leng / 2, enemy_leng - enemy_eye_leng / 2, 0);
    glm::vec3 right_russian = glm::vec3(enemy_leng - enemy_eye_leng / 2, enemy_leng - enemy_eye_leng / 2, 0);

    length4 = circle_radius - 0.08f;

    // Initialize translation steps
    translate_X_Player = 0;
    translate_Y_Player = 0;

    translate_X_Enemy = 0;
    translate_Y_Enemy = 0;

    // Proiectil Speed
    h = 0.03f;

    // fire rate
    fire_rate = 0.5f;
    // contor for timing
    time_to_fire = 0;

    //nr de obstacole
    nr_obstacole = 11;

    //pentru coliziunea cu peretii hartii
    left_x = -4.2f + circle_radius;
    right_x = 6.8f - circle_radius;
    bottom_y = -1.99 + circle_radius;
    top_y = 6.01 - circle_radius;

    // Initialize angularStep
    angular_eye = 0;

    time_enemy = 0;
    en_id = 0;
    nr_enemies = 20;  // fie 20 inamici
    Score = 0;
    HP = 3;

    Mesh* circle = object2D::CreateCircle("circle", logicSpace.width / 2, logicSpace.height / 2, circle_radius, glm::vec3(0.9843f, 0.9255f, 0.1647), true);
    AddMeshToList(circle);

    Mesh* circle_rama = object2D::CreateCircle("circle_rama", logicSpace.width / 2, logicSpace.height / 2, circle_radius, glm::vec3(0, 0, 0), false);
    AddMeshToList(circle_rama);

    Mesh* eye_left = object2D::CreateCircle("eye_left", logicSpace.width / 2 - 0.12, logicSpace.height / 2 - 0.15, eye_radius, glm::vec3(1, 0, 0), true);
    AddMeshToList(eye_left);

    Mesh* eye_right = object2D::CreateCircle("eye_right", logicSpace.width / 2 + 0.12, logicSpace.height / 2 - 0.15, eye_radius, glm::vec3(0, 0, 1), true);
    AddMeshToList(eye_right);

    Mesh* proiectil = object2D::CreateSquare("proiectil", corner, length4, glm::vec3(0, 0, 0), true);
    AddMeshToList(proiectil);

    Mesh* square = object2D::CreateSquare("square", corner, length, glm::vec3(0.361f, 0.235f, 0.573f), true);
    AddMeshToList(square);

    Mesh* rama = object2D::CreateSquare("rama", corner, length1, glm::vec3(0.635f, 0.835f, 0.776f), true);
    AddMeshToList(rama);

    Mesh* inamic = object2D::CreateSquare("inamic", { -enemy_leng / 2, -enemy_leng / 2, 0 }, enemy_leng, glm::vec3(0, 0, 1), true);
    AddMeshToList(inamic);

    Mesh* inamic_ochi_stang = object2D::CreateSquare("inamic_ochi_stang", { -enemy_eye_leng / 2, -enemy_eye_leng / 2, 0 }, enemy_eye_leng, glm::vec3(1, 1, 1), true);
    AddMeshToList(inamic_ochi_stang);

    Mesh* inamic_ochi_drept = object2D::CreateSquare("inamic_ochi_drept", { -enemy_eye_leng / 2, -enemy_eye_leng / 2, 0 }, enemy_eye_leng, glm::vec3(1, 0, 0), true);
    AddMeshToList(inamic_ochi_drept);

    Mesh* heart = object2D::CreateLive("heart", corner, glm::vec3(1, 0, 0));
    AddMeshToList(heart);

    init_obstacole_array();

}

// 2D visualization matrix
glm::mat3 Tema1::VisualizationTransf2DUnif(const LogicSpace& logicSpace, const ViewportSpace& viewSpace)
{
    float sx, sy, tx, ty, smin;
    sx = viewSpace.width / logicSpace.width;
    sy = viewSpace.height / logicSpace.height;
    if (sx < sy)
        smin = sx;
    else
        smin = sy;
    tx = viewSpace.x - smin * logicSpace.x + (viewSpace.width - smin * logicSpace.width) / 2;
    ty = viewSpace.y - smin * logicSpace.y + (viewSpace.height - smin * logicSpace.height) / 2;

    return glm::transpose(glm::mat3(
        smin, 0.0f, tx,
        0.0f, smin, ty,
        0.0f, 0.0f, 1.0f));
}

void Tema1::SetViewportArea(const ViewportSpace& viewSpace, glm::vec3 colorColor, bool clear)
{
    glViewport(viewSpace.x, viewSpace.y, viewSpace.width, viewSpace.height);

    glEnable(GL_SCISSOR_TEST);
    glScissor(viewSpace.x, viewSpace.y, viewSpace.width, viewSpace.height);

    // Clears the color buffer (using the previously set color) and depth buffer
    glClearColor(colorColor.r, colorColor.g, colorColor.b, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_SCISSOR_TEST);

    GetSceneCamera()->SetOrthographic((float)viewSpace.x, (float)(viewSpace.x + viewSpace.width), (float)viewSpace.y, (float)(viewSpace.y + viewSpace.height), 0.1f, 400);
    GetSceneCamera()->Update();
}

// Crearea unui vector ce contine toate obstacolele si dimensiunile sale
// necesare pentru "hitBOX" 
void Tema1::init_obstacole_array()
{
    obstac[0] = { 0.7f, 0.7f, length, length };
    obstac[1] = { 4.5f, 2, length1, length1 };
    obstac[2] = { 0.8f, 2, length1 * 0.3f, length1 * 1.5f };
    obstac[3] = { 0.8f, 3.5f, length2 * 1.2f, length2 * 0.15f };
    obstac[4] = { 3, 0.8f, length2 * 0.9f, length2 * 0.18f };
    obstac[5] = { 5, 6, length1 * 0.3f, length1 };
    obstac[6] = { 4.4f, 4.5f, length2, length2 * 0.13f };
    obstac[7] = { -0.5f, 2, length2 * 0.1f, length2 * 2 };
    obstac[8] = { 1, 5, length1, length1 };
    obstac[9] = { 7, 1,  length * 0.9f, 6 * length };
    obstac[10] = { 7.31f, 6.51f , length * 3, length * 3 };
}

// coliziune cerc - obstacol
bool Tema1::CheckCollision(float x, float y, float w, float h)
{
    // get center point circle first 
    glm::vec2 center(logicSpace.width / 2 + logicSpace.x, logicSpace.height / 2 + logicSpace.y);
    // calculate AABB info (center, half-extents)
    glm::vec2 aabb_half_extents(w / 2.0f, h / 2.0f);
    glm::vec2 aabb_center(
        x + aabb_half_extents.x,
        y + aabb_half_extents.y
    );
    // get difference vector between both centers
    glm::vec2 difference = center - aabb_center;
    glm::vec2 clamped = glm::clamp(difference, -aabb_half_extents, aabb_half_extents);
    // add clamped value to AABB_center and we get the value of box closest to circle
    glm::vec2 closest = aabb_center + clamped;
    // retrieve vector between center circle and closest point AABB and check if length <= radius
    difference = closest - center;
    return glm::length(difference) < circle_radius;
}

// coliziune dreptunghi - dreptunghi
bool Tema1::CheckCollision_rect(float x, float y, float w, float h)
{
    // collision x-axis?
    bool collisionX = translate_X_Proiectil + length4 >= x &&
        x + w >= translate_X_Proiectil - length4;
    // collision y-axis?
    bool collisionY = translate_Y_Proiectil + length4 >= y &&
        y + h >= translate_Y_Proiectil - length4;
    // collision only if on both axes
    return collisionX && collisionY;
}


void Tema1::FrameStart()
{
    // Clears the color buffer (using the previously set color) and depth buffer
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}


void Tema1::Update(float deltaTimeSeconds)
{
    glm::ivec2 resolution = window->GetResolution();

    viewSpace = ViewportSpace(0, 0, resolution.x, resolution.y);
    SetViewportArea(viewSpace, glm::vec3(0, 0, 0), true);

    // Compute uniform 2D visualization matrix
    visMatrix = glm::mat3(1);
    visMatrix *= VisualizationTransf2DUnif(logicSpace, viewSpace);

    // HP    
    if (HP >= 3) {
        modelMatrix = visMatrix * transform2D::Translate(0, 0);
        modelMatrix *= transform2D::Translate(logicSpace.x + 3.9f, logicSpace.y + 3);
        modelMatrix *= transform2D::Scale(0.8f, 0.8f);
        RenderMesh2D(meshes["heart"], shaders["VertexColor"], modelMatrix);

    }

    if (HP >= 2) {
        modelMatrix = visMatrix * transform2D::Translate(0, 0);
        modelMatrix *= transform2D::Translate(logicSpace.x + 4.4f, logicSpace.y + 3);
        modelMatrix *= transform2D::Scale(0.8f, 0.8f);
        RenderMesh2D(meshes["heart"], shaders["VertexColor"], modelMatrix);
    }

    if (HP >= 1) {
        modelMatrix = visMatrix * transform2D::Translate(0, 0);
        modelMatrix *= transform2D::Translate(logicSpace.x + 4.9f, logicSpace.y + 3);
        modelMatrix *= transform2D::Scale(0.8f, 0.8f);
        RenderMesh2D(meshes["heart"], shaders["VertexColor"], modelMatrix);
    }

    if (HP == 0) {
        cout << "\nPutin expands his Empire\n" << endl;
        exit(0);
    }


    //calculez unghiul dupa care trebuie sa se modifice directia jucatorului
    angular_eye = atan2(mousemoveX - (viewSpace.width / 2), (mousemoveY - (viewSpace.height / 2)));

    // 10 inamici care apar la fiecare 2 secunde la o pozitie random si cu o viteza random
    time_enemy += deltaTimeSeconds;
    if (time_enemy >= 1.5f && en_id < nr_enemies) {
        inamici[en_id] = { rand() % (int)(10 - enemy_leng) + enemy_leng / 2,rand() % (int)(8 - enemy_leng) + enemy_leng / 2, 0.002f * (rand() % 4 + 2), true };
        en_id++;
        time_enemy = 0;
    }

    // contor de timp pentru a stabili fire_rate (0.5s)
    time_to_fire += deltaTimeSeconds;

    // minimap 
    DrawScene(VisualizationTransf2DUnif({ 0, 0, 11, 8 }, { viewSpace.width * 4 / 5, 0, viewSpace.width * 1 / 5, viewSpace.width * 1 / 5 }));

    // Desenarea scenei cu toate elementele jocului
    DrawScene(visMatrix);
}


void Tema1::FrameEnd()
{
}


void Tema1::DrawScene(glm::mat3 visMatrix)
{
    //proiectil
    if (move_proiectil == 1 && proiectil_exists && proiectil_distance < max_distance) {

        translate_X_Proiectil += h * cos(angular_proiectil);
        translate_Y_Proiectil += h * sin(angular_proiectil);

        proiectil_distance += h;

        //proiectil coliziune cu peretii hartii
        if (translate_X_Proiectil >= 8.7) proiectil_exists = false;
        if (translate_X_Proiectil <= -2.2 + length4) proiectil_exists = false;
        if (translate_Y_Proiectil >= map_height - length4) proiectil_exists = false;
        if (translate_Y_Proiectil <= 0 + length4) proiectil_exists = false;

        //coliziune proiectil - obstacole
        for (int i = 0; i < nr_obstacole; i++) {
            if (CheckCollision_rect(obstac[i].x, obstac[i].y, obstac[i].w, obstac[i].h)) {
                proiectil_exists = false;
                goto continua; // ca sa nu mai faca render in plus (in perete)
            }
        }

        modelMatrix = visMatrix * transform2D::Translate(translate_X_Proiectil, translate_Y_Proiectil);
        modelMatrix *= transform2D::Rotate(angular_proiectil);
        RenderMesh2D(meshes["proiectil"], shaders["VertexColor"], modelMatrix);
    }

continua:

    /////////////// Enemy ////////////////

    // daca proiectilul nimereste in inamic
    for (int i = 0; i < en_id; i++) {
        if (inamici[i].exista) {
            if (proiectil_exists && CheckCollision_rect(inamici[i].x, inamici[i].y, enemy_leng, enemy_leng)) {
                inamici[i].exista = false;
                proiectil_exists = false;
                Score++;
                cout << "You killed " << Score << " Russians" << endl;
                if (Score + (3 - HP) == nr_enemies) {
                    cout << "\nRomania is free again, now go save Moldova !\n" << endl;
                }
            }
            // coliziune inamic - jucator
            if (CheckCollision(inamici[i].x, inamici[i].y, enemy_leng, enemy_leng)) {
                inamici[i].exista = false;
                HP--;
            }
        }
    }

    for (int i = 0; i < en_id; i++) {
        if (inamici[i].exista == true) {

            // in caz contrar inamicii continua chasingul jucatorului
            angular_enemy = atan2(translate_Y_Player - inamici[i].y + logicSpace.height / 2, translate_X_Player - inamici[i].x + logicSpace.width / 2);
            inamici[i].x += inamici[i].viteza * cos(angular_enemy);
            inamici[i].y += inamici[i].viteza * sin(angular_enemy);

            // Russian Enemy left eye
            modelMatrix = visMatrix * transform2D::Translate(inamici[i].x, inamici[i].y);
            modelMatrix *= transform2D::Rotate(angular_enemy);
            modelMatrix *= transform2D::Translate(enemy_leng / 2, enemy_leng / 2);
            RenderMesh2D(meshes["inamic_ochi_stang"], shaders["VertexColor"], modelMatrix);

            // Russian Enemy right eye
            modelMatrix = visMatrix * transform2D::Translate(inamici[i].x, inamici[i].y);
            modelMatrix *= transform2D::Rotate(angular_enemy);
            modelMatrix *= transform2D::Translate(enemy_leng / 2, -enemy_leng / 2);
            RenderMesh2D(meshes["inamic_ochi_drept"], shaders["VertexColor"], modelMatrix);

            // Russian Enemy body
            modelMatrix = visMatrix * transform2D::Translate(inamici[i].x, inamici[i].y);
            modelMatrix *= transform2D::Rotate(angular_enemy);
            RenderMesh2D(meshes["inamic"], shaders["VertexColor"], modelMatrix);

        }
    }



    // Romanian Player :)
    modelMatrix = visMatrix * transform2D::Translate(translate_X_Player, translate_Y_Player);
    RenderMesh2D(meshes["circle"], shaders["VertexColor"], modelMatrix);
    //ochiul stang
    modelMatrix = visMatrix * transform2D::Translate(translate_X_Player, translate_Y_Player);
    modelMatrix *= transform2D::Translate(logicSpace.width / 2, logicSpace.height / 2);
    modelMatrix *= transform2D::Rotate(angular_eye);
    modelMatrix *= transform2D::Translate(-(logicSpace.width / 2), -(logicSpace.height / 2));
    RenderMesh2D(meshes["eye_left"], shaders["VertexColor"], modelMatrix);
    //ochiul drept
    modelMatrix = visMatrix * transform2D::Translate(translate_X_Player, translate_Y_Player);
    modelMatrix *= transform2D::Translate(logicSpace.width / 2, logicSpace.height / 2);
    modelMatrix *= transform2D::Rotate(angular_eye);
    modelMatrix *= transform2D::Translate(-(logicSpace.width / 2), -(logicSpace.height / 2));
    RenderMesh2D(meshes["eye_right"], shaders["VertexColor"], modelMatrix);



    ///////////////////     Obstacole     //////////////////////
   //obstacol patrat

    modelMatrix = visMatrix * transform2D::Translate(0.7f, 0.7f);
    RenderMesh2D(meshes["square"], shaders["VertexColor"], modelMatrix);

    //obstacol patrat
    modelMatrix = visMatrix * transform2D::Translate(4.5f, 2);
    modelMatrix *= transform2D::Scale(2, 2);
    RenderMesh2D(meshes["square"], shaders["VertexColor"], modelMatrix);

    //obstacol dreptunghi
    modelMatrix = visMatrix * transform2D::Translate(0.8f, 2);
    modelMatrix *= transform2D::Scale(0.6f, 3);
    RenderMesh2D(meshes["square"], shaders["VertexColor"], modelMatrix);

    //obstacol dreptunghi2
    modelMatrix = visMatrix * transform2D::Translate(0.8f, 3.5f);
    modelMatrix *= transform2D::Scale(4.8f, 0.6f);
    RenderMesh2D(meshes["square"], shaders["VertexColor"], modelMatrix);

    //obstacol dreptunghi3
    modelMatrix = visMatrix * transform2D::Translate(3, 0.8f);
    modelMatrix *= transform2D::Scale(3.6f, 0.72f);
    RenderMesh2D(meshes["square"], shaders["VertexColor"], modelMatrix);

    //obstacol dreptunghi
    modelMatrix = visMatrix * transform2D::Translate(5, 6);
    modelMatrix *= transform2D::Scale(0.6f, 2);
    RenderMesh2D(meshes["square"], shaders["VertexColor"], modelMatrix);

    //obstacol dreptunghi
    modelMatrix = visMatrix * transform2D::Translate(4.4f, 4.5f);
    modelMatrix *= transform2D::Scale(4, 0.52f);
    RenderMesh2D(meshes["square"], shaders["VertexColor"], modelMatrix);

    //obstacol dreptunghi
    modelMatrix = visMatrix * transform2D::Translate(-0.5f, 2);
    modelMatrix *= transform2D::Scale(0.4f, 8);
    RenderMesh2D(meshes["square"], shaders["VertexColor"], modelMatrix);

    //obstacol patrat
    modelMatrix = visMatrix * transform2D::Translate(1, 5);
    modelMatrix *= transform2D::Scale(2, 2);
    RenderMesh2D(meshes["square"], shaders["VertexColor"], modelMatrix);

    // obstacol lung dreapta
    modelMatrix = visMatrix * transform2D::Translate(7, 1);
    modelMatrix *= transform2D::Scale(0.9f, 6);
    RenderMesh2D(meshes["square"], shaders["VertexColor"], modelMatrix);

    // obstacol patrat colt dreapta sus
    modelMatrix = visMatrix * transform2D::Translate(7.31f, 6.51f);
    modelMatrix *= transform2D::Scale(3, 3);
    RenderMesh2D(meshes["square"], shaders["VertexColor"], modelMatrix);


    // Rama 2 Vandama
    modelMatrix = visMatrix * transform2D::Scale(map_width, map_height);
    modelMatrix *= transform2D::Translate(-0.2f, 0); //harta sa fie mai la mijloc
    RenderMesh2D(meshes["rama"], shaders["VertexColor"], modelMatrix);
}

void Tema1::OnInputUpdate(float deltaTime, int mods)
{
    if (window->KeyHold(GLFW_KEY_W)) {
        logicSpace.y += deltaTime * 2.5f;

        // coliziune jucator - obstacole
        for (int i = 0; i < nr_obstacole; i++) {
            if (CheckCollision(obstac[i].x, obstac[i].y, obstac[i].w, obstac[i].h)) {
                logicSpace.y -= deltaTime * 2.5f;
            }
        }

        //coliziune jucator perete harta
        if (logicSpace.y > top_y) {
            logicSpace.y -= deltaTime * 2.5f;
        }

        translate_Y_Player = logicSpace.y;
    }

    if (window->KeyHold(GLFW_KEY_S)) {
        logicSpace.y -= deltaTime * 2.5f;

        // coliziune jucator obstacole
        for (int i = 0; i < nr_obstacole; i++) {
            if (CheckCollision(obstac[i].x, obstac[i].y, obstac[i].w, obstac[i].h)) {
                logicSpace.y += deltaTime * 2.5f;
            }
        }

        //coliziune jucator perete harta
        if (logicSpace.y < bottom_y) {
            logicSpace.y += deltaTime * 2.5f;
        }

        translate_Y_Player = logicSpace.y;
    }

    if (window->KeyHold(GLFW_KEY_A)) {
        logicSpace.x -= deltaTime * 2.5f;

        // coliziune jucator obstacole
        for (int i = 0; i < nr_obstacole; i++) {
            if (CheckCollision(obstac[i].x, obstac[i].y, obstac[i].w, obstac[i].h)) {
                logicSpace.x += deltaTime * 2.5f;
            }
        }

        //coliziune jucator perete harta
        if (logicSpace.x < left_x) {
            logicSpace.x += deltaTime * 2.5f;
        }

        translate_X_Player = logicSpace.x;
    }

    if (window->KeyHold(GLFW_KEY_D)) {
        logicSpace.x += deltaTime * 2.5f;

        // coliziune jucator obstacole
        for (int i = 0; i < nr_obstacole; i++) {
            if (CheckCollision(obstac[i].x, obstac[i].y, obstac[i].w, obstac[i].h)) {
                logicSpace.x -= deltaTime * 2.5f;
            }
        }

        //coliziune jucator perete harta
        if (logicSpace.x > right_x) {
            logicSpace.x -= deltaTime * 2.5f;
        }

        translate_X_Player = logicSpace.x;
    }
}


void Tema1::OnKeyPress(int key, int mods)
{
    // Add key press event
}


void Tema1::OnKeyRelease(int key, int mods)
{
    // Add key release event
}


void Tema1::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    // Add mouse move event
    mousemoveX = mouseX;
    mousemoveY = mouseY;

}


void Tema1::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{

}


void Tema1::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    if (button == 1) {
        // Daca a trecut 0.5s - avem voie sa impuscam, in cazz onctrar Nu
        if (time_to_fire >= fire_rate) {

            proiectil_distance = 0;
            //max distance pe care o poate zbura un proiectil
            max_distance = map_width * 2 / 5.f;

            move_proiectil = 1;
            proiectil_exists = true;

            translate_X_Proiectil = translate_X_Player + logicSpace.width / 2 - circle_radius / 3;
            translate_Y_Proiectil = translate_Y_Player + logicSpace.height / 2 - circle_radius / 3;

            angular_proiectil = atan2(mousemoveX - (viewSpace.width / 2), mousemoveY - (viewSpace.height / 2)) - (float)M_PI_2;

            // Daca am impuscat, resetam timerul
            time_to_fire = 0;
        }
    }
}


void Tema1::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}


void Tema1::OnWindowResize(int width, int height)
{
}
