#include "AppClass.h"
void Application::InitVariables(void)
{
	//Make MyMesh object
	m_pMesh = new MyMesh();
	m_pMesh->GenerateCube(2.0f, C_BROWN);

	//Make MyMesh object
	m_pMesh1 = new MyMesh();
	m_pMesh1->GenerateCube(1.0f, C_WHITE);

	static int p_mCubeCount = 46;

	for (int i = 0; i < 46; ++i)
	{
		myMeshCollection.push_back(new MyMesh());
		myMeshCollection[i]->GenerateCube(1.0f, C_BLACK);
	}
}
void Application::Update(void)
{
	//Update the system so it knows how much time has passed since the last call
	m_pSystem->Update();

	//Is the arcball active?
	ArcBall();

	//Is the first person camera active?
	CameraRotation();
}
void Application::Display(void)
{
	// Clear the screen
	ClearScreen();

	matrix4 m4view = m_pCameraMngr->GetViewMatrix();
	matrix4 m4project = m_pCameraMngr->GetProjectionMatrix();

	matrix4 mScale = glm::scale(IDENTITY_M4, vector3(2.0f, 2.0f, 2.0f));
	matrix4 mTranslate = glm::translate(vector3(1.0f, 0.0f, 0.0f));
	static float value = 0.0;
	value += 0.01;

	matrix4 m4model = mTranslate;
	m4model = glm::scale(vector3(2.0f, 2.0f, 2.0f));

	m_pMesh->Render(m4project, m4view, m4model);

	// moving all 46 cubes
	for (int i = 0; i < myMeshCollection.size(); i++)
	{
		matrix4 mMoveRight = glm::translate(vector3(1.0f * i + value, 1.0f * i, 0.0f));

		myMeshCollection[i]->Render(m4project, m4view, mMoveRight);
	}

	//m_pMesh->Render(m_pCameraMngr->GetProjectionMatrix(), m_pCameraMngr->GetViewMatrix(), ToMatrix4(m_qArcBall));
	//m_pMesh1->Render(m_pCameraMngr->GetProjectionMatrix(), m_pCameraMngr->GetViewMatrix(), glm::translate(vector3( 3.0f, 0.0f, 0.0f)));
		
	// draw a skybox
	m_pMeshMngr->AddSkyboxToRenderList();
	
	//render list call
	m_uRenderCallCount = m_pMeshMngr->Render();

	//clear the render list
	m_pMeshMngr->ClearRenderList();
	
	//draw gui
	DrawGUI();
	
	//end the current frame (internally swaps the front and back buffers)
	m_pWindow->display();
}
void Application::Release(void)
{
	if (m_pMesh != nullptr)
	{
		delete m_pMesh;
		m_pMesh = nullptr;
	}
	SafeDelete(m_pMesh1);

	// deallocate memory in the entire mesh collection when finished
	for (int i = 0; i < myMeshCollection.size(); i++)
	{
		SafeDelete(myMeshCollection[i]);
	}

	//release GUI
	ShutdownGUI();
}