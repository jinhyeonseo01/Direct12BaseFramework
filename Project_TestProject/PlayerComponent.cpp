#include "PlayerComponent.h"

#include "Bullet.h"
#include "DXEngine.h"
#include "GameObject.h"
#include "Mesh.h"
#include "Transform.h"




bool PlayerComponent::isMenu = true;
bool PlayerComponent::isMenuHelp = false;


PlayerComponent::~PlayerComponent()
{
}

void* PlayerComponent::Clone()
{
    return Component::Clone();
}

void PlayerComponent::ReRef()
{
    Component::ReRef();
}

void PlayerComponent::Destroy()
{
    Component::Destroy();
}

void PlayerComponent::Init()
{
    Component::Init();
}

void PlayerComponent::Start()
{
    Component::Start();

    std::vector<std::shared_ptr<GameObject>> targets;
    gameObject.lock()->rootParent.lock()->GetChildsAllByName(L"Propellers", targets);
    topRoter = targets[0];

    targets.clear();
    gameObject.lock()->rootParent.lock()->GetChildsAllByName(L"Back_Propeller", targets);
    backRoter.push_back(targets[0]);
    targets.clear();
    gameObject.lock()->rootParent.lock()->GetChildsAllByName(L"Back_Propeller.001", targets);
    backRoter.push_back(targets[0]);

    targets.clear();
    gameObject.lock()->scene.lock()->Finds(L"Camera", targets, false);
    camera = targets[0];


    targets.clear();
    gameObject.lock()->scene.lock()->Finds(L"B2", targets, false);
    auto t = targets[0];
    targets.clear();
    t->GetChildsAllByName(L"default", targets);
    ter = targets[0];

    hmT = ResourceManager::main->GetTexture(L"hm");


    targets.clear();
    gameObject.lock()->scene.lock()->Finds(L"menu", targets, false);
    menu = targets[0];

    targets.clear();
    gameObject.lock()->scene.lock()->Finds(L"menu_start", targets, false);
    menu_start = targets[0];

    targets.clear();
    gameObject.lock()->scene.lock()->Finds(L"menu_help", targets, false);
    menu_help = targets[0];

    targets.clear();
    gameObject.lock()->scene.lock()->Finds(L"menu_info", targets, false);
    menu_info = targets[0];
}

void PlayerComponent::Update()
{
    Component::Update();
    if (isPlayer)
    {
        if (Input::main->GetMouseDown(KeyCode::LeftMouse) && isMenuHelp)
        {
            isMenuHelp = false;
        }
        if (Input::main->GetKeyDown(KeyCode::I))
            isMenuHelp = !isMenuHelp;
        menu_info.lock()->transform->localPosition.z = isMenuHelp ? 0.01f : 2.0f;

        if (Input::main->GetKeyDown(KeyCode::Num1))
        {
            std::vector<std::shared_ptr<MeshRenderer>> MRs;
            gameObject.lock()->GetComponentsWithChilds<MeshRenderer>(MRs);
            for (auto mr : MRs)
            {
                mr->materialList.clear();
                std::shared_ptr<Material> material = std::make_shared<Material>();
                material->shader = ResourceManager::main->GetShader(L"forward");
                material->SetData("color", Vector4(0, 0, 1, 1));
                material->SetData("_BaseMap", ResourceManager::main->GetTexture(L"ApacheTexture"));
                mr->AddMateiral({ material });
            }
        }
        if (Input::main->GetKeyDown(KeyCode::Num2))
        {
            std::vector<std::shared_ptr<MeshRenderer>> MRs;
            gameObject.lock()->GetComponentsWithChilds<MeshRenderer>(MRs);
            for (auto mr : MRs)
            {
                mr->materialList.clear();
                std::shared_ptr<Material> material = std::make_shared<Material>();
                material->shader = ResourceManager::main->GetShader(L"forward");
                material->SetData("color", Vector4(0, 0, 1, 1));
                material->SetData("_BaseMap", ResourceManager::main->GetTexture(L"ApacheTexture2"));
                mr->AddMateiral({ material });
            }
        }
        if (Input::main->GetKeyDown(KeyCode::Num3))
        {
            std::vector<std::shared_ptr<MeshRenderer>> MRs;
            gameObject.lock()->GetComponentsWithChilds<MeshRenderer>(MRs);
            for (auto mr : MRs)
            {
                mr->materialList.clear();
                std::shared_ptr<Material> material = std::make_shared<Material>();
                material->shader = ResourceManager::main->GetShader(L"forward");
                material->SetData("color", Vector4(0, 0, 1, 1));
                material->SetData("_BaseMap", ResourceManager::main->GetTexture(L"ApacheTexture3"));
                mr->AddMateiral({ material });
            }
        }
        if (Input::main->GetKeyDown(KeyCode::Num4))
        {
            std::vector<std::shared_ptr<MeshRenderer>> MRs;
            gameObject.lock()->GetComponentsWithChilds<MeshRenderer>(MRs);
            for (auto mr : MRs)
            {
                mr->materialList.clear();
                std::shared_ptr<Material> material = std::make_shared<Material>();
                material->shader = ResourceManager::main->GetShader(L"forward");
                material->SetData("color", Vector4(0, 0, 1, 1));
                material->SetData("_BaseMap", ResourceManager::main->GetTexture(L"ApacheTexture4"));
                mr->AddMateiral({ material });
            }
        }

    }
    if(isMenu)
    {
        if (Input::main->GetMouseDown(KeyCode::LeftMouse) && isPlayer)
        {
            auto mousePos = Input::main->GetMousePosition();
            mousePos = Vector2(
                mousePos.x / GraphicManager::main->setting.screenInfo.width,
                mousePos.y / GraphicManager::main->setting.screenInfo.height);
            mousePos.y = 1 - mousePos.y;
            mousePos = mousePos*2 - Vector3::One;
            //mousePos.x *= (GraphicManager::main->setting.screenInfo.width / GraphicManager::main->setting.screenInfo.height);

            auto startMenu = gameObject.lock()->scene.lock()->Find(L"menu_start");
            auto startMenuMin = startMenu->transform->LocalToWorld_Position(-Vector3(0.5, 0.5, 0));
            auto startMenuMax = startMenu->transform->LocalToWorld_Position(Vector3(0.5, 0.5, 0));
            if (startMenuMin.x <= mousePos.x && startMenuMax.x >= mousePos.x &&
                startMenuMin.y <= mousePos.y && startMenuMax.y >= mousePos.y)
            {
                isMenu = false;
                isMenuHelp = false;
                menu.lock()->transform->localPosition.z = isMenu ? 0.1f : 2.0f;
                menu_start.lock()->transform->localPosition.z = isMenu ? 0.01f : 2.0f;
                menu_help.lock()->transform->localPosition.z = isMenu ? 0.01f : 2.0f;
            }

            if(isMenuHelp)
            {
                isMenuHelp = false;

                menu_info.lock()->transform->localPosition.z = isMenuHelp ? 0.01f : 2.0f;
            }

            startMenu = gameObject.lock()->scene.lock()->Find(L"menu_help");
            startMenuMin = startMenu->transform->LocalToWorld_Position(-Vector3(0.5, 0.5, 0));
            startMenuMax = startMenu->transform->LocalToWorld_Position(Vector3(0.5, 0.5, 0));
            if (startMenuMin.x <= mousePos.x && startMenuMax.x >= mousePos.x &&
                startMenuMin.y <= mousePos.y && startMenuMax.y >= mousePos.y)
            {
                isMenuHelp = true;
                menu.lock()->transform->localPosition.z = isMenu ? 0.1f : 2.0f;
                menu_start.lock()->transform->localPosition.z = isMenu ? 0.01f : 2.0f;
                menu_help.lock()->transform->localPosition.z = isMenu ? 0.01f : 2.0f;
                menu_info.lock()->transform->localPosition.z = isMenuHelp ? 0.01f : 2.0f;
            }
        }
    }
    else
    {
        if (Input::main->GetKeyDown(KeyCode::Esc) && isPlayer)
        {
            isMenu = true;
            menu.lock()->transform->localPosition.z = isMenu ? 0.1f : 2.0f;
            menu_start.lock()->transform->localPosition.z = isMenu ? 0.01f : 2.0f;
            menu_help.lock()->transform->localPosition.z = isMenu ? 0.01f : 2.0f;
            menu_info.lock()->transform->localPosition.z = isMenuHelp ? 0.01f : 2.0f;
        }
        if (Input::main->GetMouseDown(KeyCode::LeftMouse) && isPlayer)
        {
            auto rootObject = gameObject.lock()->scene.lock()->CreateGameObject(L"Apache");

            auto bullet = gameObject.lock()->scene.lock()->CreateGameObjects(ResourceManager::main->GetModel(L"box"));
            bullet->transform->localScale = Vector3(0.2,0.2,1) * 1.000f;
            bullet->SetParent(rootObject);
            auto bulletComp = rootObject->AddComponent<Bullet>();

            rootObject->transform->worldPosition(gameObject.lock()->transform->worldPosition() + gameObject.lock()->transform->forward() * 0.2f);
            rootObject->transform->worldRotation(gameObject.lock()->transform->worldRotation());
            std::vector<std::shared_ptr<MeshRenderer>> meshRenderers;
            bullet->GetComponentsWithChilds(meshRenderers);
            for (int i = 0; i < meshRenderers.size(); i++)
            {
                std::shared_ptr<Material> material = std::make_shared<Material>();
                material->shader = ResourceManager::main->GetShader(L"forward");
                material->SetData("color", Vector4(0, 0, 1, 1));
                material->SetData("_BaseMap", ResourceManager::main->GetTexture(L"ApacheTexture2"));
                meshRenderers[i]->AddMateiral({ material });
            }

        }


        //애니메이션 로직
        topRoter.lock()->transform->localRotation = topRoter.lock()->transform->localRotation * Quaternion::CreateFromYawPitchRoll(-Engine::GetMainEngine()->deltaTime * 30, 0, 0);
        for (auto& back : backRoter)
            back.lock()->transform->localRotation = back.lock()->transform->localRotation * Quaternion::CreateFromYawPitchRoll(0, Engine::GetMainEngine()->deltaTime * 20, 0);

        //각도 로직
        auto worldDir = ((-gameObject.lock()->transform->forward()) * 3.0f + gameObject.lock()->transform->up() * 1.0f);
        if(isPlayer)
            camera.lock()->transform->worldPosition(gameObject.lock()->transform->worldPosition() + worldDir);

        auto dir = ((gameObject.lock()->transform->worldPosition() + gameObject.lock()->transform->forward() * 4) - camera.lock()->transform->worldPosition());
        dir.Normalize(dir);
        if (isPlayer)
        camera.lock()->transform->LookUp(dir, Vector3(0, 1, 0));//gameObject.lock()->transform->up()

        //이동 로직
        auto r = gameObject.lock()->transform->right() * ((Input::main->GetKey(KeyCode::D) ? 1 : 0) - (Input::main->GetKey(KeyCode::A) ? 1 : 0));
        r += gameObject.lock()->transform->forward() * ((Input::main->GetKey(KeyCode::W) ? 1 : 0) - (Input::main->GetKey(KeyCode::S) ? 1 : 0));


        if (!isPlayer)
        {
            r = gameObject.lock()->transform->forward();
            time -= Engine::GetMainEngine()->deltaTime;
            if(time < 0)
            {
                time = 5 + (rand() % 1000) / 1000.0 * 5;
                gameObject.lock()->transform->worldRotation(Quaternion::CreateFromYawPitchRoll((rand() % 1000) / 1000.0 * 360, 0, 0));
            }
            auto pos = gameObject.lock()->transform->worldPosition();
            if (pos.x > 50)
                pos.x -= 100;
            if (pos.z > 50)
                pos.z -= 100;

            if (pos.x < -50)
                pos.x += 100;
            if (pos.z < -50)
                pos.z += 100;
        }



        r.Normalize(r);
        velocity += Engine::GetMainEngine()->deltaTime * r;
        gameObject.lock()->transform->worldPosition(gameObject.lock()->transform->worldPosition() + velocity);
        velocity = velocity * ((100 - (Engine::GetMainEngine()->deltaTime * 60) * 5) / 100.0);

        //터레인 로직
        auto localPos = ter.lock()->transform->WorldToLocal_Position(gameObject.lock()->transform->worldPosition());
        Vector4 color = hmT.lock()->GetPixel(
            (std::round(std::abs(localPos.x)) / 10000) * hmT.lock()->_size.x,
            (1 - std::round(std::abs(localPos.z)) / 10000) * hmT.lock()->_size.y, 1
        );
        if (localPos.y - (color.x * 10000) <= 10)
            localPos.y = (color.x * 10000) + 11;
        gameObject.lock()->transform->worldPosition(
            ter.lock()->transform->LocalToWorld_Position(localPos));
        //Debug::log << localPos.y - (color.x * 10000) <<"\n";

        Vector3 forward;
        gameObject.lock()->transform->forward().Normalize(forward);
        if (Equals(velocity.Length(), 0))
            velocity = gameObject.lock()->transform->forward() * 0.01f;
        velocity.Normalize(forward);
        Ray ray = Ray(gameObject.lock()->transform->worldPosition(), forward);
        for(auto box : boxs)
        {
            for(auto mesh : box.lock()->meshList)
            {
                float dis = 10000;
                Vector3 normal;
                if(mesh.lock()->Intersects(box.lock()->gameObject.lock()->transform, ray, dis, normal))
                {
                    if(dis >= 0 && dis <= 1)
                    {
                        Vector3 objForward = velocity;
                        normal.Normalize(normal);
                        velocity = (objForward - normal.Dot(objForward) * 1.02 * normal) * velocity.Length();
                        gameObject.lock()->transform->forward(objForward - normal.Dot(objForward)*1.02 * normal);
                        angle = gameObject.lock()->transform->worldRotation().ToEuler();
                    }
                    else if (dis >= 0 && dis <= 3)
                    {
                        Vector3 objForward = velocity;
                        normal.Normalize(normal);
                        Vector3 nextForward = (objForward - normal.Dot(objForward) * 1.02 * normal);
                        float a = Engine::GetMainEngine()->deltaTime * 60 * 0.1;
                        nextForward = (nextForward * a + objForward * (1-a));
                        nextForward.Normalize(nextForward);
                        velocity = nextForward * velocity.Length();
                        gameObject.lock()->transform->forward(nextForward);
                        angle = gameObject.lock()->transform->worldRotation().ToEuler();
                    }

                }
            }
        }
        
        //회전 로직
        if (isPlayer)
        if (Input::main->GetMouseDown(KeyCode::RightMouse))
        {
            cameraControl = !cameraControl;
            prevPos = Input::main->GetMousePosition();
            //ShowCursor(cameraControl ? TRUE : FALSE);
            Engine::GetMainEngine()->SetCursorHide(cameraControl);
        }
        if (isPlayer)
        if (cameraControl)//Input::main->GetMouse(KeyCode::RightMouse) && 
        {
            auto pos = (Input::main->GetMousePosition() - prevPos);
            auto movePos = (((Vector3(0, 1, 0) * pos.x) + (Vector3(1, 0, 0) * pos.y)) * 0.003f);

            angle += movePos;
            angle.z = 0;
            if (std::abs(angle.x) > 50 * D2R)
                angle.x = std::sign(angle.x) * 50 * D2R;
            gameObject.lock()->transform->worldRotation(Quaternion::CreateFromYawPitchRoll(angle));
            //gameObject.lock()->transform->LookUp(movePos, Vector3(0, 1, 0));
            prevPos = Input::main->GetMousePosition();
        }
    }
}

void PlayerComponent::LateUpdate()
{
    Component::LateUpdate();
}

void PlayerComponent::OnEnable()
{
    Component::OnEnable();
}

void PlayerComponent::OnDisable()
{
    Component::OnDisable();
}

void PlayerComponent::OnDestroy()
{
    Component::OnDestroy();
}

void PlayerComponent::OnComponentDestroy()
{
    Component::OnComponentDestroy();
}

void PlayerComponent::BeforeRendering()
{
    Component::BeforeRendering();
}

void PlayerComponent::Rendering()
{
    Component::Rendering();
}

void PlayerComponent::AfterRendering()
{
    Component::AfterRendering();
}
