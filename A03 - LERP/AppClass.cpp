#include "AppClass.h"
void Application::InitVariables(void)
{
	//Change this to your name and email
	m_sProgrammer = "Justin Armstrong - jxa1762@rit.edu";
	
	//Set the position and target of the camera
	//(I'm at [0,0,10], looking at [0,0,0] and up is the positive Y axis)
	m_pCameraMngr->SetPositionTargetAndUpward(AXIS_Z * 20.0f, ZERO_V3, AXIS_Y);

	//if the light position is zero move it
	if (m_pLightMngr->GetPosition(1) == ZERO_V3)
		m_pLightMngr->SetPosition(vector3(0.0f, 0.0f, 3.0f));

	//if the background is cornflowerblue change it to black (its easier to see)
	if (vector3(m_v4ClearColor) == C_BLUE_CORNFLOWER)
	{
		m_v4ClearColor = vector4(ZERO_V3, 1.0f);
	}
	
	//if there are no segments create 7
	if(m_uOrbits < 1)
		m_uOrbits = 7;

	float fSize = 1.0f; //initial size of orbits
	float fRadius = 0.95f; // initial orbit radius

	//creating a color using the spectrum 
	uint uColor = 650; //650 is Red
	//prevent division by 0
	float decrements = 250.0f / (m_uOrbits > 1? static_cast<float>(m_uOrbits - 1) : 1.0f); //decrement until you get to 400 (which is violet)
	/*
		This part will create the orbits, it start at 3 because that is the minimum subdivisions a torus can have
	*/
	uint uSides = 3; //start with the minimal 3 sides
	for (uint i = uSides; i < m_uOrbits + uSides; i++)
	{
		vector3 v3Color = WaveLengthToRGB(uColor); //calculate color based on wavelength
		m_shapeList.push_back(m_pMeshMngr->GenerateTorus(fSize, fSize - 0.1f, 3, i, v3Color)); //generate a custom torus and add it to the meshmanager
		//std::vector<vector3> stopsList; // "add a list of stops to the list of lists of stops"
		fSize += 0.5f; //increment the size for the next orbit
		uColor -= static_cast<uint>(decrements); //decrease the wavelength
	}
	
	fSize = 1.0f;
	uSides = 3;
	for (size_t i = 0; i < m_uOrbits; i++)
	{
		std::vector<vector3> points;
		for (size_t i = 0; i < uSides; i++)
		{
			double radians = i * (2 * PI / uSides);
			vector3 vCircle = vector3(sin(radians)*fSize, cos(radians)*fSize, 0);
			points.push_back(vCircle);
		}
		uSides++;
		fSize += 0.5f;
		m_stopsList.push_back(points);
		m_routeControl.push_back(0);
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

	// set up clock/timer information
	static float fTimer = 0.0;
	static uint uClock = m_pSystem->GenClock();
	fTimer += m_pSystem->GetDeltaTime(uClock);

	/*vector3 v3CurrentPosition;

	// percentage var
	float fPercentage = static_cast<double>(MapValue(dTimer, 0.0, 1.0, 0.0, 1.0));

	// Do these have to be inside the loop?
	vector3 v3Start;
	vector3 v3End;
	v3Start = m_stopsList[route][route];
	v3End = m_stopsList[(route + 1) % m_stopsList.size()][(route + 1) % m_stopsList.size()];

	v3CurrentPosition = glm::lerp(v3Start, v3End, fPercentage);*/
 
	matrix4 m4View = m_pCameraMngr->GetViewMatrix(); //view Matrix
	matrix4 m4Projection = m_pCameraMngr->GetProjectionMatrix(); //Projection Matrix
	matrix4 m4Offset = IDENTITY_M4; //offset of the orbits, starts as the global coordinate system
	/*
		The following offset will orient the orbits as in the demo, start without it to make your life easier.
	*/
	m4Offset = glm::rotate(IDENTITY_M4, 1.5708f, AXIS_Z);

	//float fPercentage = static_cast<double>(MapValue(fTimer, 0.0, 1.0, 0.0, 1.0));
	uint uSides = 3;
	static uint routeA = 0;
	vector3 v3S;
	vector3 v3E;
	vector3 v3CurrentPos;
	matrix4 m4Model;

	// draw a shapes
	for (uint i = 0; i < m_uOrbits; ++i)
	{
		m_pMeshMngr->AddMeshToRenderList(m_shapeList[i], glm::rotate(m4Offset, 1.5708f, AXIS_X));

		if (m_routeControl[i] >= 1) // start: make the position to reach the very first point
			v3E = m_stopsList[i][m_routeControl[i] - 1];
		else 
			v3E = m_stopsList[i][m_stopsList[i].size() - 1];

		if (m_routeControl[i] < m_stopsList[i].size()) // check if routeControl exceeds the stop list size
		{
			// calculate sphere's position
			v3CurrentPos = glm::lerp(v3E, m_stopsList[i][m_routeControl[i]], fTimer);
			if (length(v3CurrentPos - m_stopsList[i][m_routeControl[i]]) <= .05f) { // when .05f away, change to the next point
				for (size_t j = 0; j < m_routeControl.size(); j++)
				{
					m_routeControl[j]++;
				}
				fTimer = 0;
			}
		}
		else {
			m_routeControl[i] = 0;
			v3CurrentPos = v3E;
		}

		m4Model = glm::translate(v3CurrentPos); // moving the spheres based in the LERP
		m_pMeshMngr->AddSphereToRenderList(m4Model * glm::scale(vector3(0.1)), C_WHITE); // create the spheres

		uSides++;
	}

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
	//release GUI
	ShutdownGUI();
}