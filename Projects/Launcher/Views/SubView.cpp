#include "pch.h"
#include "Subview.h"

#include "Engine/Objects/Entities/UI/UIButton.h"
#include "Engine/Objects/Components/Transform/Transform2DComponent.h"
#include "ListView.h"

#include "../PhotoEntity.h"

using namespace Engine;

SubView::SubView(const String& Path, const String& Name) : World(Name)
{
	m_Path = Path;

	// Create photo view
	m_Photo = Add<PhotoEntity>(Path);
	m_CameraImage = m_Photo->GetPhoto();

	// Back button
	constexpr float BtnSize = 50.f;

	const String Icon = "icons/icon_back_w.png";

	m_BackButton = Add<UIButton>(
		(ButtonStyle) {
		"", 5.f, Vector4(45.f), "", Icon, Color(0.f, 0.f, 0.f, 0.5f)
	},
		(ButtonStyle) {
		"", 5.f, Vector4(45.f), "", Icon, Color(0.f, 0.f, 0.f, 0.5f)
	},
		(ButtonStyle) {
		"", 5.f, Vector4(45.f), "", Icon, Color(0.882352941f, 0.2f, 0.203921569f)
	},
		"Back Button"
	);

	m_BackButton->GetForeground()->GetComponent<Transform2DComponent>()->SetScale(Vector2(0.75f));

	m_BackButton->SetAnchor(E_ANCH_TOP_LEFT);
	m_BackButton->SetPivot(Vector2(0.0f, 0.0f));

	m_BackButton->GetComponent<Transform2DComponent>()->Translate(Vector2(20.f, 20.f));

	m_BackButton->SetOnClickedCallback([&]() {
		Application::Get().ThreadedCallback.Bind(this, &SubView::OnBack);
	});

	m_KeyHandler = Application::Get().GetRenderContext().GetWindow().OnKeyPressed.Bind(
		this, &SubView::OnKeyPressed
	);
}

SubView::~SubView()
{
	Application::Get().GetRenderContext().GetWindow().OnKeyPressed.Unbind(m_KeyHandler);
}

void SubView::OnKeyPressed(int32_t Key, int32_t Repeat)
{
	if (!IsActive())
		return;

	if (Key == GLFW_KEY_BACK)
		m_BackButton->Click();
}

void SubView::OnBack()
{
	delete this;
}
