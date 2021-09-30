#include "pch.h"
#include "ListView.h"

#include "LineView.h"
#include "CameraView.h"

#include "Engine/Application.h"

#include "Engine/Objects/Worlds/Entities/StaticEntity.h"

#include "Engine/Objects/Worlds/Entities/Components/UI/Layouts/Canvas/UIGridPanel.h"
#include "Engine/Objects/Worlds/Entities/Components/UI/Renderables/Shapes/UIOval.h"
#include "Engine/Objects/Worlds/Entities/Components/UI/Renderables/UIText.h"

#include "Engine/Objects/Worlds/Entities/UI/UIButton.h"
#include "PhotoView.h"

using namespace Engine;

ListView::ListView()
{
	StaticEntity* Canvas = new StaticEntity("Canvas");
	
	UILayout* Layout = Canvas->AddComponent<UIGridPanel>();
	Layout->SetElementCount(1, true);
	Layout->SetFitElements(true, false);

	auto& Window = Application::Get().GetRenderContext().GetWindow();
	
	AABB Bounds = Layout->GetBounds();

	float Width = Bounds.width() / Window.GetScale();
	float Height = 100.f;// (Bounds.fBottom - Bounds.fTop) * 0.2f / Window.GetScale();

	std::vector<String> Filenames;
	m_FolderPath = "/storage/emulated/0/DCIM/Appuil/" + Application::Get().GetName() + "/";

#ifdef CB_PLATFORM_WINDOWS
	Filenames = {
		"Item 0.jpg", "Item 1.jpg", "Item 2.jpg", "Item 3.jpg", "Item 4.jpg",
		"Item 0.jpg", "Item 1.jpg", "Item 2.jpg", "Item 3.jpg", "Item 4.jpg"
	};
#else
	Filenames = FileLoader::GetFilenames(m_FolderPath, { "jpg" }, FileLoader::E_ROOT);
#endif

	// Take photo item
	Color BgColor = (
		Color(1.f, 1.f, 1.f)
	);

	UIButton* Item = new UIButton(
		{ "Nieuwe foto maken", 0.f, Vector4(80.f), "", "", BgColor },
		{ "Nieuwe foto maken", 0.f, Vector4(80.f), "", "", Color(1.f, 1.f, 0.f) },
		{ "Nieuwe foto maken", 0.f, Vector4(80.f), "", "", Color(1.f, 0.f, 0.f) },
		"Item Header"
	);

	Item->SetAnchor(E_ANCH_TOP_FILL);
	Item->SetPivot(Vector2(0.5f, 0.f));

	Item->SetParent(Canvas);

	Item->SetOnClickedCallback([&] {
		Application::Get().ThreadedCallback.Bind(this, &ListView::OnCameraClick);
	});

	// Items
	uint32_t Num = Filenames.size();
	for (uint32_t i = 0; i < Num; ++i)
	{
		BgColor = (
			//glm::mod(glm::floor((i + 1) * 0.5), 2.0) == 0.0 ?
			i % 2 == 0.0 ?
			Color(0.95f, 0.95f, 0.95f) :
			Color(1.f, 1.f, 1.f)
		);

		Item = new UIButton(
			{ Filenames[i], 0.f, Vector4(80.f), "", "", BgColor },
			{ Filenames[i], 0.f, Vector4(80.f), "", "", Color(1.f, 1.f, 0.f) },
			{ Filenames[i], 0.f, Vector4(80.f), "", "", Color(1.f, 0.f, 0.f) },
			"Item " + std::to_string(i)
		);

		Item->SetParent(Canvas);

		Item->SetAnchor(E_ANCH_TOP_FILL);
		Item->SetPivot(Vector2(0.5f, 0.f));
		
		String Fullpath = m_FolderPath + Filenames[i];

		Item->SetOnClickedCallback([this, Fullpath] {
			m_ImagePath = Fullpath; // m_FolderPath + Item->GetDefaultStyle().m_Text;
			Application::Get().ThreadedCallback.Bind(this, &ListView::OnPhotoView);
		});
	}
}

void ListView::OnCameraClick()
{
	Application::Get().GetWorldManager().Remove(
		Application::Get().GetWorldManager().GetActive()
	);

	CameraView* View = new CameraView();
}

void ListView::OnPhotoView()
{
	// Store because this object will soon be destroyed
	String Path = m_ImagePath;

	Application::Get().GetWorldManager().Remove(
		Application::Get().GetWorldManager().GetActive()
	);

	PhotoView* View = new PhotoView(Path);
}

void ListView::Draw(float fDeltaTime)
{
	// Draw layers and objects
	World::Draw(fDeltaTime);
}