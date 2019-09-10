// cLocalization.cpp

#include <Deployment\cLocalization.h>
#include <XML\pugixml.hpp>

cLocalization::cLocalization()
{
	this->selectedLanguage = eLanguages::INSTRUCTIONS;
	this->_basePath = "assets/xml/";

	//this->InitFreetype();

	return;
}

cLocalization::~cLocalization()
{
	return;
}

void cLocalization::Init(GLuint shaderProgramID)
{
	this->InitFreetype();
	//this->SetBasePath("assets/localization/");
	//this->LoadTextFromXML("localization.xml");
	//generate and bind vbo 
	glGenBuffers(1, &this->mdp_vbo);
	//this->SetShaderUniLocs(shaderProgramID);
}

bool cLocalization::InitFreetype()
{
	if (FT_Init_FreeType(&this->mft))
	{
		fprintf(stderr, "Error to init free type!\n");
		return false;
	}

	if (FT_New_Face(this->mft, "assets/fonts/FreeSans.ttf", 0, &this->mface))
	{
		fprintf(stderr, "Error opening font!\n");
		return false;
	}

	//set font size
	FT_Set_Pixel_Sizes(this->mface, 0, 48);


	if (FT_Load_Char(this->mface, 'X', FT_LOAD_RENDER))
	{
		fprintf(stderr, "Error in loading character!\n");
		return false;
	}


	return true;
}

bool cLocalization::SetBasePath(std::string basePath)
{
	// Check if the file path exists
	struct stat info;
	if (stat(basePath.c_str(), &info) != 0)
	{
		printf("Localization path %s couldn't be accessed!\n", basePath);
		return false;
	}
	else if (info.st_mode& S_IFDIR)
	{
		// Found, set it to be used
		this->_basePath = basePath;
		printf("Localization path set.\n");
		return true;
	}

	return false;
}

void cLocalization::SetShaderUniLocs(GLuint shaderProgramID)
{
	this->bIsText_Uniloc = glGetUniformLocation(shaderProgramID, "bIsText");
	this->tCoord_Uniloc = glGetAttribLocation(shaderProgramID, "tCoord");
	this->text_Uniloc = glGetUniformLocation(shaderProgramID, "tText");
	this->textColor_Uniloc = glGetUniformLocation(shaderProgramID, "textColor");

	//generate and bind vao
	//glGenVertexArrays(1, &this->mvao);
}

bool cLocalization::LoadTextFromXML(std::string filename)
{
	std::string fullPath = this->_basePath;
	fullPath.append(filename);

	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file(fullPath.c_str());

	if (result.status != pugi::status_ok)
	{
		printf("Error loading %s XML file!\n", filename.c_str());
		return false;
	}

	pugi::xml_node_iterator iter = doc.children().begin();
	for (iter; iter != doc.children().end(); iter++) // This is the root object
	{
		pugi::xml_node_iterator iter2 = iter->children().begin();
		for (iter2; iter2 != iter->children().end(); iter2++) // These are the language objects
		{
			pugi::xml_node_iterator iter3 = iter2->children().begin();
			for (iter3; iter3 != iter2->children().end(); iter3++) // These are the strings inside each language object
			{
				// Add each language string to the map
				this->mapLanguageText[iter2->first_attribute().value()].push_back(iter3->child_value());
			}
		}
	}

	return true;
}

void cLocalization::WriteText(std::string text)
{
	printf("%s\n", text.c_str());
}

void cLocalization::RenderText(std::string text, float x, float y, float sx, float sy)
{
	//glClearColor(1, 1, 1, 1);
	//glClear(GL_COLOR_BUFFER_BIT);

	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);	// Default

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glUniform1f(this->bIsText_Uniloc, (float)GL_TRUE);
	glUniform4fv(this->textColor_Uniloc, 1, black);

	const char *p;
	FT_GlyphSlot g = this->mface->glyph;

	GLuint tex;

	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glUniform1i(this->text_Uniloc, 0);

	/* Clamping to edges is important to prevent artifacts when scaling */
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	/* Linear filtering usually looks best for text */
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	/* We require 1 byte alignment when uploading texture data */
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	/* Set up the VBO for our vertex data */
	glEnableVertexAttribArray(this->tCoord_Uniloc);
	glBindBuffer(GL_ARRAY_BUFFER, this->mdp_vbo);
	glVertexAttribPointer(this->tCoord_Uniloc, 4, GL_FLOAT, GL_FALSE, 0, 0);


	/* Loop through all characters */
	for (p = text.c_str(); *p; p++)
	{
		/* Try to load and render the character */
		if (FT_Load_Char(this->mface, *p, FT_LOAD_RENDER))
		{
			continue;
		}		

		/* Upload the "bitmap", which contains an 8-bit grayscale image, as an alpha texture */
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, g->bitmap.width, g->bitmap.rows, 0, GL_ALPHA, GL_UNSIGNED_BYTE, g->bitmap.buffer);
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, g->bitmap.width, g->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, g->bitmap.buffer);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, g->bitmap.width, g->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, g->bitmap.buffer);

		/* Calculate the vertex and texture coordinates */
		float x2 = x + g->bitmap_left * sx;
		float y2 = -y - g->bitmap_top * sy;
		float w = g->bitmap.width * sx;
		float h = g->bitmap.rows * sy;

		glm::vec4 box[4] = 
		{
			{ x2, -y2, 0, 0 },
			{ x2 + w, -y2, 1, 0 },
			{ x2, -y2 - h, 0, 1 },
			{ x2 + w, -y2 - h, 1, 1 },
		};

		
		/* Draw the character on the screen */
		glBufferData(GL_ARRAY_BUFFER, sizeof(box), box, GL_DYNAMIC_DRAW);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		/* Advance the cursor to the start of the next character */
		x += (g->advance.x >> 6) * sx;
		y += (g->advance.y >> 6) * sy;
	}
	

	//glDisable(GL_BLEND);

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	glUniform1f(this->bIsText_Uniloc, (float)GL_FALSE);
	glDisableVertexAttribArray(this->tCoord_Uniloc);
	glDeleteTextures(1, &tex);
}

void cLocalization::RenderMenu(float width, float height)
{
	std::vector<std::string> vecStrings;

	switch (this->selectedLanguage)
	{
	case ENGLISH:
		vecStrings = this->mapLanguageText["EN"];
		break;
	case PORTUGUESE:
		vecStrings = this->mapLanguageText["PT"];
		break;
	case SPANISH:
		vecStrings = this->mapLanguageText["SP"];
		break;
	case HYLIAN:
		vecStrings = this->mapLanguageText["HL"];
		break;
	case HUE:
		vecStrings = this->mapLanguageText["HU"];
		break;
	case INSTRUCTIONS:
		vecStrings = this->mapLanguageText["IN"];
		break;
	default:
		break;
	}

	float sx = 2.0f / width;
	float sy = 2.0f / height;
	GLfloat yoffset = 50.0f;
	GLfloat xoffset = 8 * sx;

	for (std::vector<std::string>::iterator iter = vecStrings.begin(); iter != vecStrings.end(); iter++)
	{
		std::string tmp = *iter;
		//this->RenderText(tmp.c_str(), -1 + xoffset, 1 - yoffset * sy, sx, sy);
		this->WriteText(tmp.c_str());
		yoffset += 50.0f;
	}
	printf("\n");
}

void cLocalization::ProcessKeys(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) // "forward"
	{
		if (this->selectedLanguage != eLanguages::ENGLISH)
		{
			this->selectedLanguage = eLanguages::ENGLISH;
			this->RenderMenu(0.0f, 0.0f);
		}
	}
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) // "forward"
	{
		if (this->selectedLanguage != eLanguages::PORTUGUESE)
		{
			this->selectedLanguage = eLanguages::PORTUGUESE;
			this->RenderMenu(0.0f, 0.0f);
		}
	}
	if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) // "forward"
	{
		if (this->selectedLanguage != eLanguages::SPANISH)
		{
			this->selectedLanguage = eLanguages::SPANISH;
			this->RenderMenu(0.0f, 0.0f);
		}
	}
	if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS) // "forward"
	{
		if (this->selectedLanguage != eLanguages::HYLIAN)
		{
			this->selectedLanguage = eLanguages::HYLIAN;
			this->RenderMenu(0.0f, 0.0f);
		}
	}
	if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS) // "forward"
	{
		if (this->selectedLanguage != eLanguages::HUE)
		{
			this->selectedLanguage = eLanguages::HUE;
			this->RenderMenu(0.0f, 0.0f);
		}
	}
}
