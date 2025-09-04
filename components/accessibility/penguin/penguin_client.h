// Copyright 2024 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef COMPONENTS_ACCESSIBILITY_PENGUIN_PENGUIN_CLIENT_H_
#define COMPONENTS_ACCESSIBILITY_PENGUIN_PENGUIN_CLIENT_H_

#include <string>

#include "base/functional/callback.h"
#include "base/functional/callback_forward.h"
#include "build/build_config.h"

#if BUILDFLAG(IS_ANDROID)
#include "base/android/jni_string.h"
#include "base/android/scoped_java_ref.h"
#endif

namespace content {
class BrowserContext;
class WebContents;
}  // namespace content

namespace gfx {
class Rect;
}  // namespace gfx

namespace penguin {

// Use this callback to get only the main text from the API call.
// The callback will be invoked with the extracted main content text from the
// API response. If the API call fails, an empty string will be passed.
using PenguinCompactResponseCallback =
    base::OnceCallback<void(const std::string&)>;

// Use this callback format to get all information from the API call, which
// will include response codes and all parts of the response (possibly
// multiple candidates, finish reason, safety ratings, etc.).
// Parameters:
//   response_code: HTTP response code from the API call (200 for success)
//   net_error_code: Network error code (net::OK for success)
//   main_content: The main text content extracted from the API response
using PenguinFullResponseCallback =
    base::OnceCallback<void(int32_t response_code,
                            int32_t net_error_code,
                            const std::string& main_content)>;

// PenguinClient provides access to accessibility-focused AI services for
// analyzing web content, images, and text. It offers multiple API variants
// to support different input types and response formats.
//
// The client supports:
// - Text-only analysis
// - Image + text analysis 
// - Web content extraction and analysis
// - Android JNI integration for cross-platform support
//
// Usage Example:
//   auto* client = PenguinClient::Create(browser_context);
//   if (client) {
//     client->PerformAPICall("Describe this image", 
//                           base::BindOnce(&OnCompactResponse));
//   }
//
// All API calls are asynchronous and results are delivered via callbacks.
// The client handles network requests, error handling, and response parsing
// internally.
class PenguinClient {
 public:
  // Factory method to create a PenguinClient instance.
  // Returns nullptr if the client cannot be created for the given context.
  // The returned pointer is owned by the caller.
  //
  // Parameters:
  //   context: The browser context to associate with this client. Must not
  //            be null and should remain valid for the lifetime of the client.
  static PenguinClient* Create(content::BrowserContext* context);

  // Constructs a PenguinClient for the given browser context.
  // Note: Use Create() factory method instead of direct construction.
  //
  // Parameters:
  //   context: The browser context to associate with this client.
  explicit PenguinClient(content::BrowserContext* context);

  ~PenguinClient();
  PenguinClient(const PenguinClient& client) = delete;
  PenguinClient& operator=(const PenguinClient& client) = delete;

  // Performs an API call with both image data and text input.
  // This is useful for image description, OCR, or multimodal analysis tasks.
  //
  // Parameters:
  //   image_data: Base64-encoded image data or image URL
  //   text_input: Text prompt or question about the image
  //   callback: Invoked with the main text response when the call completes
  void PerformAPICall(const std::string& image_data,
                      const std::string& text_input,
                      PenguinCompactResponseCallback callback);
                      
  // Performs an API call with both image data and text input.
  // Returns detailed response information including status codes.
  //
  // Parameters:
  //   image_data: Base64-encoded image data or image URL
  //   text_input: Text prompt or question about the image  
  //   callback: Invoked with response code, net error, and content
  void PerformAPICall(const std::string& image_data,
                      const std::string& text_input,
                      PenguinFullResponseCallback callback);

  // Performs an API call using content from a web page.
  // Extracts relevant content from the WebContents and analyzes it.
  //
  // Parameters:
  //   web_contents: The web page to analyze. Must not be null.
  //   text_input: Text prompt or question about the web page content
  //   callback: Invoked with the main text response when the call completes
  void PerformAPICall(content::WebContents* web_contents,
                      const std::string& text_input,
                      PenguinCompactResponseCallback callback);
                      
  // Performs an API call using content from a web page.
  // Returns detailed response information including status codes.
  //
  // Parameters:
  //   web_contents: The web page to analyze. Must not be null.
  //   text_input: Text prompt or question about the web page content
  //   callback: Invoked with response code, net error, and content
  void PerformAPICall(content::WebContents* web_contents,
                      const std::string& text_input,
                      PenguinFullResponseCallback callback);

  // Performs an API call using content from a specific region of a web page.
  // Extracts content from the specified rectangle area only.
  //
  // Parameters:
  //   web_contents: The web page to analyze. Must not be null.
  //   text_input: Text prompt or question about the web page content
  //   callback: Invoked with the main text response when the call completes
  //   source_rect: Rectangle defining the area of interest on the page
  void PerformAPICall(content::WebContents* web_contents,
                      const std::string& text_input,
                      PenguinCompactResponseCallback callback,
                      const gfx::Rect& source_rect);
                      
  // Performs an API call using content from a specific region of a web page.
  // Returns detailed response information including status codes.
  //
  // Parameters:
  //   web_contents: The web page to analyze. Must not be null.
  //   text_input: Text prompt or question about the web page content
  //   callback: Invoked with response code, net error, and content
  //   source_rect: Rectangle defining the area of interest on the page
  void PerformAPICall(content::WebContents* web_contents,
                      const std::string& text_input,
                      PenguinFullResponseCallback callback,
                      const gfx::Rect& source_rect);

  // Performs a text-only API call.
  // Useful for general text analysis, question answering, or text generation.
  //
  // Parameters:
  //   text_input: The text to analyze or prompt for the API
  //   callback: Invoked with the main text response when the call completes
  void PerformAPICall(const std::string& text_input,
                      PenguinCompactResponseCallback callback);
                      
  // Performs a text-only API call.
  // Returns detailed response information including status codes.
  //
  // Parameters:
  //   text_input: The text to analyze or prompt for the API
  //   callback: Invoked with response code, net error, and content
  void PerformAPICall(const std::string& text_input,
                      PenguinFullResponseCallback callback);

#if BUILDFLAG(IS_ANDROID)
  // Android equivalent methods that are called through JNI (cannot be
  // overloaded). These methods bridge between Java/Kotlin code and the
  // native C++ implementation.
  
  // Android JNI variant for image + text analysis.
  // Called from Java with Base64-encoded image data and text input.
  //
  // Parameters:
  //   env: JNI environment pointer
  //   j_image_data: Java string containing Base64-encoded image data
  //   j_text_input: Java string with the text prompt
  //   j_callback: Java callback object to receive the response
  //   j_include_full_response: If true, provides full response details
  void PerformAPICall_var1(
      JNIEnv* env,
      const base::android::JavaParamRef<jstring>& j_image_data,
      const base::android::JavaParamRef<jstring>& j_text_input,
      const base::android::JavaParamRef<jobject>& j_callback,
      jboolean j_include_full_response);

  // Android JNI variant for web content analysis.
  // Called from Java with a WebContents object and text input.
  //
  // Parameters:
  //   env: JNI environment pointer
  //   j_web_contents: Java WebContents object to analyze
  //   j_text_input: Java string with the text prompt
  //   j_callback: Java callback object to receive the response
  //   j_include_full_response: If true, provides full response details
  void PerformAPICall_var2(
      JNIEnv* env,
      const base::android::JavaParamRef<jobject>& j_web_contents,
      const base::android::JavaParamRef<jstring>& j_text_input,
      const base::android::JavaParamRef<jobject>& j_callback,
      jboolean j_include_full_response);

  // Android JNI variant for web content analysis with source rectangle.
  // Called from Java to analyze a specific region of a web page.
  //
  // Parameters:
  //   env: JNI environment pointer
  //   j_web_contents: Java WebContents object to analyze
  //   j_text_input: Java string with the text prompt
  //   j_callback: Java callback object to receive the response
  //   j_source_rect: Java Rect object defining the analysis region
  //   j_include_full_response: If true, provides full response details
  void PerformAPICall_var3(
      JNIEnv* env,
      const base::android::JavaParamRef<jobject>& j_web_contents,
      const base::android::JavaParamRef<jstring>& j_text_input,
      const base::android::JavaParamRef<jobject>& j_callback,
      const base::android::JavaParamRef<jobject>& j_source_rect,
      jboolean j_include_full_response);

  // Android JNI variant for text-only analysis.
  // Called from Java with only text input.
  //
  // Parameters:
  //   env: JNI environment pointer
  //   j_text_input: Java string with the text to analyze
  //   j_callback: Java callback object to receive the response
  //   j_include_full_response: If true, provides full response details
  void PerformAPICall_var4(
      JNIEnv* env,
      const base::android::JavaParamRef<jstring>& j_text_input,
      const base::android::JavaParamRef<jobject>& j_callback,
      jboolean j_include_full_response);
#endif
};

}  // namespace penguin

#endif  // COMPONENTS_ACCESSIBILITY_PENGUIN_PENGUIN_CLIENT_H_
