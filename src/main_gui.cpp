#define WIN32_LEAN_AND_MEAN
#define UNICODE
#define _UNICODE
#include <windows.h>
#include <commdlg.h>
#include <shlobj.h>
#include <objbase.h>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <filesystem>
#include "tokenizer.hpp"
#include "file_scanner.hpp"
#include "document_store.hpp"
#include "index.hpp"
#include "search.hpp"

using namespace notesearch;

// global state - TODO: maybe save/load index to file later?
static DocumentStore g_doc_store;
static InvertedIndex g_index;
static std::vector<SearchResult> g_current_results;

// control IDs
#define ID_SEARCH_EDIT       1001
#define ID_SEARCH_BUTTON     1002
#define ID_INDEX_BUTTON       1003
#define ID_RESULTS_LIST       1004  // not used yet, might add listbox later
#define ID_STATUS_TEXT        1005
#define ID_RESULTS_EDIT       1006

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
std::wstring StringToWString(const std::string& str);
std::string WStringToString(const std::wstring& wstr);
void UpdateStatus(HWND hwnd, const std::string& message);
void DisplayResults(HWND hwnd, const std::vector<SearchResult>& results);
void IndexDirectory(HWND hwnd, const std::filesystem::path& dir_path);
void PerformSearch(HWND hwnd, const std::string& query);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // need COM for folder picker dialog
    CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    
    const wchar_t CLASS_NAME[] = L"NoteSearchWindow";
    
    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    
    if (!RegisterClass(&wc)) {
        // should check error but whatever
        return 0;
    }
    
    // create main window - size is kinda arbitrary
    HWND hwnd = CreateWindowEx(
        0,
        CLASS_NAME,
        L"NoteSearch - Full-Text Search Engine",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 900, 600,
        NULL, NULL, hInstance, NULL
    );
    
    if (hwnd == NULL) {
        return 0;
    }
    
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);
    
    // main message loop
    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
    CoUninitialize();
    
    return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_CREATE: {
            // search label
            CreateWindow(L"STATIC", L"Search Query:",
                WS_VISIBLE | WS_CHILD,
                10, 10, 100, 20,
                hwnd, NULL, NULL, NULL);
            
            // search box
            CreateWindow(L"EDIT", L"",
                WS_VISIBLE | WS_CHILD | WS_BORDER | ES_LEFT,
                10, 35, 600, 25,
                hwnd, (HMENU)ID_SEARCH_EDIT, NULL, NULL);
            
            // search button
            CreateWindow(L"BUTTON", L"Search",
                WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                620, 33, 100, 30,
                hwnd, (HMENU)ID_SEARCH_BUTTON, NULL, NULL);
            
            // index button
            CreateWindow(L"BUTTON", L"Index Directory...",
                WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                730, 33, 150, 30,
                hwnd, (HMENU)ID_INDEX_BUTTON, NULL, NULL);
            
            // status text
            CreateWindow(L"STATIC", L"Status: Ready",
                WS_VISIBLE | WS_CHILD,
                10, 70, 880, 20,
                hwnd, (HMENU)ID_STATUS_TEXT, NULL, NULL);
            
            CreateWindow(L"STATIC", L"Results:",
                WS_VISIBLE | WS_CHILD,
                10, 100, 100, 20,
                hwnd, NULL, NULL, NULL);
            
            // results area - edit control works fine, listbox might be better but this is simpler
            HWND hResults = CreateWindow(L"EDIT", L"",
                WS_VISIBLE | WS_CHILD | WS_BORDER | WS_VSCROLL | WS_HSCROLL | 
                ES_MULTILINE | ES_READONLY | ES_AUTOVSCROLL | ES_AUTOHSCROLL,
                10, 125, 870, 430,
                hwnd, (HMENU)ID_RESULTS_EDIT, NULL, NULL);
            
            // set monospace font for results
            HFONT hFont = CreateFont(
                16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Consolas"
            );
            SendMessage(hResults, WM_SETFONT, (WPARAM)hFont, TRUE);
            
            UpdateStatus(hwnd, "Ready - Click 'Index Directory...' to start");
            return 0;
        }
        
        case WM_COMMAND: {
            WORD id = LOWORD(wParam);
            
            if (id == ID_SEARCH_BUTTON) {
                HWND hEdit = GetDlgItem(hwnd, ID_SEARCH_EDIT);
                int length = GetWindowTextLength(hEdit);
                if (length > 0) {
                    std::vector<wchar_t> buffer(length + 1);
                    GetWindowText(hEdit, buffer.data(), length + 1);
                    std::string query = WStringToString(buffer.data());
                    PerformSearch(hwnd, query);
                }
                return 0;
            }
            
            if (id == ID_INDEX_BUTTON) {
                // folder picker
                BROWSEINFO bi = {};
                bi.hwndOwner = hwnd;
                bi.lpszTitle = L"Select Directory to Index";
                bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;
                
                LPITEMIDLIST pidl = SHBrowseForFolder(&bi);
                if (pidl != NULL) {
                    wchar_t path[MAX_PATH];
                    if (SHGetPathFromIDList(pidl, path)) {
                        std::filesystem::path dir_path(path);
                        IndexDirectory(hwnd, dir_path);
                    }
                    CoTaskMemFree(pidl);
                }
                return 0;
            }
            
            // enter key in search box
            if (id == ID_SEARCH_EDIT && HIWORD(wParam) == EN_CHANGE) {
                // could add live search here but not needed
                // maybe later if I have time
            }
            return 0;
        }
        
        case WM_KEYDOWN: {
            if (wParam == VK_RETURN) {
                HWND hFocus = GetFocus();
                if (hFocus == GetDlgItem(hwnd, ID_SEARCH_EDIT)) {
                    // Simulate search button click
                    SendMessage(hwnd, WM_COMMAND, 
                        MAKEWPARAM(ID_SEARCH_BUTTON, BN_CLICKED), 0);
                    return 0;
                }
            }
            break;
        }
        
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }
    
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void UpdateStatus(HWND hwnd, const std::string& message) {
    HWND hStatus = GetDlgItem(hwnd, ID_STATUS_TEXT);
    std::wstring wmsg = StringToWString("Status: " + message);
    SetWindowText(hStatus, wmsg.c_str());
}

void DisplayResults(HWND hwnd, const std::vector<SearchResult>& results) {
    HWND hResults = GetDlgItem(hwnd, ID_RESULTS_EDIT);
    
    std::wstringstream wss;
    
    if (results.empty()) {
        wss << L"No results found.\n";
    } else {
        wss << L"Found " << results.size() << L" result(s):\n\n";
        
        for (size_t i = 0; i < results.size(); ++i) {
            const auto& result = results[i];
            wss << L"[" << (i + 1) << L"] " << StringToWString(result.path) << L"\n";
            wss << L"    Score: " << std::fixed << std::setprecision(4) << result.score << L"\n";
            if (!result.snippet.empty()) {
                wss << L"    " << StringToWString(result.snippet) << L"\n";
            }
            wss << L"\n";
        }
    }
    
    std::wstring content = wss.str();
    SetWindowText(hResults, content.c_str());
}

void IndexDirectory(HWND hwnd, const std::filesystem::path& dir_path) {
    UpdateStatus(hwnd, "Scanning directory...");
    UpdateWindow(hwnd);  // force update
    
    auto start = std::chrono::high_resolution_clock::now();
    
    FileScanner scanner;
    auto files = scanner.scan_directory(dir_path);
    
    UpdateStatus(hwnd, "Indexing " + std::to_string(files.size()) + " files...");
    UpdateWindow(hwnd);
    
    // clear old index
    g_doc_store.clear();
    g_index.clear();
    
    // index files
    for (const auto& file_pair : files) {
        const auto& file_path = file_pair.first;
        const auto& content = file_pair.second;
        uint32_t doc_id = g_doc_store.add_document(file_path, content);
        auto tokens = tokenize(content);
        g_index.index_document(doc_id, tokens);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    std::stringstream ss;
    ss << "Indexed " << g_doc_store.size() << " documents, " 
       << g_index.vocabulary_size() << " unique terms in " 
       << duration.count() << " ms";
    UpdateStatus(hwnd, ss.str());
    
    DisplayResults(hwnd, {});  // clear results
}

void PerformSearch(HWND hwnd, const std::string& query) {
    if (g_doc_store.empty()) {
        UpdateStatus(hwnd, "Error: No documents indexed. Please index a directory first.");
        MessageBox(hwnd, L"No documents indexed. Please click 'Index Directory...' first.", 
                   L"Search Error", MB_OK | MB_ICONWARNING);
        return;
    }
    
    if (query.empty()) {
        return;  // nothing to search
    }
    
    UpdateStatus(hwnd, "Searching...");
    UpdateWindow(hwnd);
    
    auto start = std::chrono::high_resolution_clock::now();
    
    SearchEngine engine(g_index, g_doc_store);
    g_current_results = engine.search(query, 20);  // limit to 20 results
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    DisplayResults(hwnd, g_current_results);
    
    std::stringstream ss;
    ss << "Found " << g_current_results.size() << " result(s) in " 
       << duration.count() << " ms";
    UpdateStatus(hwnd, ss.str());
}

std::wstring StringToWString(const std::string& str) {
    if (str.empty()) return std::wstring();
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
    std::wstring wstrTo(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
    return wstrTo;
}

std::string WStringToString(const std::wstring& wstr) {
    if (wstr.empty()) return std::string();
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
    std::string strTo(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
    return strTo;
}
