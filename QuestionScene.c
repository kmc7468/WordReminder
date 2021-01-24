#include "Window.h"

#include "UIEngine.h"

static HWND g_DescriptionStatic;
static HWND g_QuestionStatic, g_HintStatic;

static HWND g_Selectors[5];
static HWND g_PronunciationSelectors[5];

static HWND g_StopButton;

LRESULT CALLBACK QuestionSceneProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam, UINT_PTR dummy0, DWORD_PTR dummy1) {
	EVENT {
	case AM_CREATE:
		g_DescriptionStatic = CreateStatic(_T("설명란입니다"), WS_VISIBLE | SS_CENTER, handle, -1);
		g_QuestionStatic = CreateStatic(_T("문제란입니다"), WS_VISIBLE | SS_CENTER, handle, -1);
		g_HintStatic = CreateStatic(_T("힌트란입니다"), WS_VISIBLE | SS_CENTER, handle, -1);

		for (int i = 0; i < 5; ++i) {
			g_Selectors[i] = CreateButton(_T("선지입니다"), WS_VISIBLE, handle, i);
			g_PronunciationSelectors[i] = CreateButton(NULL, 0, handle, i + 5);
		}

		g_StopButton = CreateButton(_T("그만 외우기"), WS_VISIBLE, handle, 10);
		return 0;

	case AM_CREATEUI: {
		UIEngine* const uiEngine = (UIEngine*)lParam;

		UIFONT_CON(buttonFont, 18, true);
		UIFONT_FCN(questionFont, 40, true);
		UIFONT_FCN(hintFont, 22, false);
		UIFONT_FCN(selectorFont, 18, false);

		UICOMP_DOH(section1, Horizontal, None, 100, &uiEngine->RootComponent);
		UIMARG_CON_AS(section1, 10);

		UICOMP_WIN(section2, None, section1);
		UIMARG_CON(section2, Bottom, -60);

		UICOMP_DOC(questionSection, Horizontal, None, section2);
		UICOMP_CON_W(descriptionStatic, &g_DescriptionStatic, Horizontal, None, 23, buttonFont, questionSection);
		UIMARG_CON(descriptionStatic, Bottom, 5);
		UICOMP_FCN_W(questionStatic, &g_QuestionStatic, Horizontal, None, 40, questionFont, questionSection);
		UICOMP_CON(hintStaticMargin1, Horizontal, None, 5, questionSection);
		UICOMP_FCN_W(hintStatic, &g_HintStatic, Horizontal, None, 22, hintFont, questionSection);
		UICOMP_CON(hintStaticMargin2, Horizontal, None, 10, questionSection);

		UICOMP_WIN(selectorSection1, Center, section2);
		UICOMP_DOW(selectorSection2, Vertical, Center, 50, selectorSection1);

		UICOMP_DOC(selector1Section, Horizontal, None, selectorSection2);
		UICOMP_DOC_N(selector1, _T("Selector1"), Horizontal, None, selectorFont, selector1Section);
		UICOMP_FCN(selector1Text, Horizontal, None, 36, selector1);
		UICOMP_CON(selector1Body, Horizontal, None, 14, selector1);
		UICOMP_CON(selector1Margin, Horizontal, None, 10, selector1Section);
		UICOMP_DOC(selector2Section, Horizontal, None, selectorSection2);
		UICOMP_DOC_N(selector2, _T("Selector2"), Horizontal, None, selectorFont, selector2Section);
		UICOMP_FCN(selector2Text, Horizontal, None, 36, selector2);
		UICOMP_CON(selector2Body, Horizontal, None, 14, selector2);
		UICOMP_CON(selector2Margin, Horizontal, None, 10, selector2Section);
		UICOMP_DOC(selector3Section, Horizontal, None, selectorSection2);
		UICOMP_DOC_N(selector3, _T("Selector3"), Horizontal, None, selectorFont, selector3Section);
		UICOMP_FCN(selector3Text, Horizontal, None, 36, selector3);
		UICOMP_CON(selector3Body, Horizontal, None, 14, selector3);
		UICOMP_CON(selector3Margin, Horizontal, None, 10, selector3Section);
		UICOMP_DOC(selector4Section, Horizontal, None, selectorSection2);
		UICOMP_DOC_N(selector4, _T("Selector4"), Horizontal, None, selectorFont, selector4Section);
		UICOMP_FCN(selector4Text, Horizontal, None, 36, selector4);
		UICOMP_CON(selector4Body, Horizontal, None, 14, selector4);
		UICOMP_CON(selector4Margin, Horizontal, None, 10, selector4Section);
		UICOMP_DOC(selector5Section, Horizontal, None, selectorSection2);
		UICOMP_DOC_N(selector5, _T("Selector5"), Horizontal, None, selectorFont, selector5Section);
		UICOMP_FCN(selector5Text, Horizontal, None, 36, selector5);
		UICOMP_CON(selector5Body, Horizontal, None, 14, selector5);

		selector1->Window = &g_Selectors[0];
		selector2->Window = &g_Selectors[1];
		selector3->Window = &g_Selectors[2];
		selector4->Window = &g_Selectors[3];
		selector5->Window = &g_Selectors[4];

		UICOMP_WIN(buttonSection, None, section1);
		UIMARG_CON(buttonSection, Top, 10);

		UICOMP_DOW_N(buttonSection11, _T("ButtonSection-1/1"), Vertical, None, 100, buttonFont, buttonSection);
		UICOMP_DOW_N(buttonSection12, _T("ButtonSection-1/2"), Vertical, None, 50, buttonFont, buttonSection);
		UIMARG_CON(buttonSection12, Right, 5);
		UICOMP_DOW_N(buttonSection22, _T("ButtonSection-2/2"), Vertical, None, 50, buttonFont, buttonSection);
		UIMARG_CON(buttonSection22, Left, 5);

		buttonSection11->Window = &g_StopButton;
		return 0;
	}

	case AM_ACTIVATE:
		SetSceneTitle(handle, _T("단어 암기하기"));
		return 0;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case 10:
			// TODO
			break;
		}
		return 0;

	default:
		return DefSubclassProc(handle, message, wParam, lParam);
	}
}