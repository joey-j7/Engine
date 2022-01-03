#include "pch.h"
#include "ListView.h"

#include "LineView.h"
#include "CameraView.h"

#include "Engine/Application.h"

#include "Engine/Objects/Components/UI/Layouts/Canvas/UIGridPanel.h"
#include "Engine/Objects/Components/UI/Renderables/Shapes/UIOval.h"
#include "Engine/Objects/Components/UI/Renderables/UIText.h"

#include "PhotoView.h"

#include "Platform/PermissionManager.h"
#include "Rendering/Renderers/Renderer2D.h"

using namespace Engine;

float ListView::m_ScrollPosition = 0.f;

ListView::ListView(const String& Name) : World(Name)
{
	// Set background color
	Application::Get().GetRenderContext().GetAPI().GetRenderer2D()->SetClearColor(
		Color(
			0.929411765f,
			0.937254902f,
			0.933333333f
		)
	);

	// Generate list
	m_Canvas = Add<Entity>("Canvas");

	m_Layout = m_Canvas->AddComponent<UIGridPanel>();
	
	m_Canvas->GetComponent<Transform2DComponent>()->SetPosition(Vector2(0.f, 50.f));

	m_Layout->SetGridCount(4, 0);
	m_Layout->AlignElementsToGrid(true, false);

	// Set default image path
	m_FolderPath = "/storage/emulated/0/DCIM/Appuil/" + Application::Get().GetName() + "/";

	PopulateList();
	CreateHeader();

	m_Layout->SetScrollPosition(Vector2(0.f, m_ScrollPosition));
}

bool ListView::Play()
{
	if (!World::Play())
		return false;

	// Set background color
	Application::Get().GetRenderContext().GetAPI().GetRenderer2D()->SetClearColor(
		Color(
			0.929411765f,
			0.937254902f,
			0.933333333f
		)
	);

	return true;
}

void ListView::OnCameraClick()
{
	m_ScrollPosition = 0.f;
	WorldManager::Get().Push<CameraView>();
}

void ListView::OnPhotoView()
{
	m_ScrollPosition = m_Layout->GetScrollPosition().y;

	// Store because this object will soon be destroyed
	String Path = m_ImagePath;
	WorldManager::Get().Push<PhotoView>(Path);
}

void ListView::CreateHeader()
{
	const String Icon = "icons/icon_back.png";

	// New photo button
	UIButton* BackButton = Add<UIButton>(
		(ButtonStyle) {
		"", 0.f, Vector4(50.f), "", Icon, Color(
			0.929411765f,
			0.937254902f,
			0.933333333f
		)
	},
		(ButtonStyle) {
		"", 0.f, Vector4(50.f), "", Icon, Color(1.f)
	},
		(ButtonStyle) {
		"", 0.f, Vector4(50.f), "", Icon, Color(0.882352941f, 0.2f, 0.203921569f)
	},
		"Back Button"
	);

	BackButton->GetForeground()->GetComponent<Transform2DComponent>()->SetScale(Vector2(0.75f));

	BackButton->SetAnchor(E_ANCH_TOP_LEFT);
	BackButton->SetPivot(Vector2(0.f, 0.f));

	BackButton->SetOnClickedCallback([&] {
		Application::Get().ThreadedCallback.Bind(this, &ListView::OnCameraClick);
	});

	BackButton->GetForeground()->GetComponent<Transform2DComponent>()->SetScale(Vector2(0.75f));
}

void ListView::PopulateList()
{
	Color BgColor = (
		Color(1.f, 1.f, 1.f)
	);

	// Clear old items
	for (auto ListItem : m_ListItems)
	{
		delete ListItem;
	}

	m_ListItems.clear();

	// Populate new items
	std::vector<String> Filenames;

#ifdef CB_PLATFORM_WINDOWS
	Filenames = {
		"Item 0.jpg", "Item 1.jpg", "Item 2.jpg", "Item 3.jpg", "Item 4.jpg",
		"Item 0.jpg", "Item 1.jpg", "Item 2.jpg", "Item 3.jpg", "Item 4.jpg"
	};
#else
	Filenames = FileLoader::GetFilenames(m_FolderPath, { "jpg" }, FileLoader::E_ROOT);
#endif

	Window& Window = Application::Get().GetRenderContext().GetWindow();
	const float Width = Window.GetWidth() / Window.GetScale();

	// Items
	uint32_t Num = Filenames.size();
	for (uint32_t i = 0; i < Num; ++i)
	{
		BgColor = Color(1.f, 1.f, 1.f, 0.f);
		String Fullpath = m_FolderPath + Filenames[i];
		
		PhotoListItem* Item = Add<PhotoListItem>(
			(String)Fullpath,
			(ButtonStyle) {
			"", 0.f, Vector4(Width * 0.25f), "", "", BgColor
		},
			(ButtonStyle) {
			"", 0.f, Vector4(Width * 0.25f), "", "", Color(1.f)
		},
			(ButtonStyle) {
			"", 0.f, Vector4(Width * 0.25f), "", "", Color(0.882352941f, 0.2f, 0.203921569f)
		});

		Item->SetParent(m_Canvas);

		Item->SetOnClickedCallback([this, Fullpath] {
			m_ImagePath = Fullpath; // m_FolderPath + Item->GetDefaultStyle().m_Text;
			Application::Get().ThreadedCallback.Bind(this, &ListView::OnPhotoView);
		});

		m_ListItems.push_back(Item);
	}
}

void ListView::Draw(float fDeltaTime)
{
	// Draw layers and objects
	World::Draw(fDeltaTime);
}