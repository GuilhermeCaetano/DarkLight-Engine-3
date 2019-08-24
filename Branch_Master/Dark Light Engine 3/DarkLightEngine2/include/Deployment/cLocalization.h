#ifndef _cLocalization_HG_
#define _cLocalization_HG_

#include <ft2build.h>
#include FT_FREETYPE_H

#include <string>
#include <vector>
#include <map>
#include <OpenGL\OpenGLHeaders.h>

enum eLanguages
{
	ENGLISH,
	PORTUGUESE,
	SPANISH,
	HYLIAN,
	HUE,
	INSTRUCTIONS
};

class cLocalization
{
public:
	cLocalization();
	~cLocalization();

	void Init(GLuint shaderProgramID);

	bool InitFreetype();

	bool SetBasePath(std::string basePath);

	void SetShaderUniLocs(GLuint shaderProgramID);

	// Loads text from an XML file.
	// Returns true on completion and false on error
	bool LoadTextFromXML(std::string filename);

	void WriteText(std::string text);

	// Renders text to the screen using freetype
	void RenderText(std::string text, float x, float y, float sx, float sy);

	// Renders a menu on the screen using freetype
	void RenderMenu(float width, float height);

	void ProcessKeys(GLFWwindow* window);

	// Window aspects for text rendering
	FT_Library mft;
	FT_Face mface;
	GLboolean bIsText_Uniloc;
	GLint tCoord_Uniloc;
	GLint text_Uniloc;
	GLint textColor_Uniloc;

	GLfloat black[4] = { 1, 0, 0, 1 };

	//vertex array object
	GLuint mvao;
	//vertex buffer object
	GLuint mdp_vbo;

	eLanguages selectedLanguage;
	std::map < std::string/*Language Code*/, std::vector<std::string>/*text*/> mapLanguageText;

private:
	std::string _basePath;

};


#endif // !_cLocalization_HG_

