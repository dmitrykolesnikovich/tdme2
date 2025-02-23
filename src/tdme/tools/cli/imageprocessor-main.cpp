#include <string>

#include <tdme/tdme.h>
#include <tdme/application/Application.h>
#include <tdme/engine/fileio/textures/PNGTextureWriter.h>
#include <tdme/engine/fileio/textures/Texture.h>
#include <tdme/engine/fileio/textures/TextureReader.h>
#include <tdme/engine/Version.h>
#include <tdme/os/filesystem/FileSystem.h>
#include <tdme/os/filesystem/FileSystemInterface.h>
#include <tdme/utilities/Console.h>
#include <tdme/utilities/Exception.h>

using std::string;

using tdme::application::Application;
using tdme::engine::fileio::textures::PNGTextureWriter;
using tdme::engine::fileio::textures::Texture;
using tdme::engine::fileio::textures::TextureReader;
using tdme::engine::Version;
using tdme::os::filesystem::FileSystem;
using tdme::os::filesystem::FileSystemInterface;
using tdme::utilities::Console;
using tdme::utilities::Exception;

int main(int argc, char** argv)
{
	Console::println(string("imageprocessor ") + Version::getVersion());
	Console::println(Version::getCopyright());
	Console::println();

	//
	if (argc != 3) {
		Console::println("Usage: imageprocessor input.png output.png");
		Application::exit(1);
	}

	//
	auto inputImageFileName = string(argv[1]);
	auto outputImageFileName = string(argv[2]);

	//
	try {
		Console::println("Loading image: " + inputImageFileName);
		auto image = TextureReader::read(
			FileSystem::getInstance()->getPathName(inputImageFileName),
			FileSystem::getInstance()->getFileName(inputImageFileName)
		);

		//
		Console::println("Processing image");

		// for now: do black pixel -> transparent pixels, every other pixel gets white
		//	later we can provide color transform matrices with preset matrices
		auto bytesPerPixel = image->getDepth() / 8;
		for (auto y = 0; y < image->getTextureHeight(); y++) {
			for (auto x = 0; x < image->getTextureWidth(); x++) {
				auto offset = y * bytesPerPixel * image->getTextureWidth() + x * bytesPerPixel;
				auto red = image->getTextureData()->get(offset + 0);
				auto green = image->getTextureData()->get(offset + 1);
				auto blue = image->getTextureData()->get(offset + 2);
				auto alpha = bytesPerPixel == 4?image->getTextureData()->get(offset + 3):0xff;
				// transform black pixels to transparent pixels
				if (red < 5 && green < 5 && blue < 5) {
					alpha = 0;
				} else {
					// everything else should be white
					red = 0xff;
					green = 0xff;
					blue = 0xff;
				}
				image->getTextureData()->getBuffer()[offset + 0] = red;
				image->getTextureData()->getBuffer()[offset + 1] = green;
				image->getTextureData()->getBuffer()[offset + 2] = blue;
				if (bytesPerPixel == 4) {
					image->getTextureData()->getBuffer()[offset + 3] = alpha;
				}

			}
		}

		// smooth
		auto smoothedTexture = TextureReader::smooth(image);
		image->releaseReference();
		image = smoothedTexture;

		//
		Console::println("Saving image: " + outputImageFileName);
		PNGTextureWriter::write(
			image,
			FileSystem::getInstance()->getPathName(outputImageFileName),
			FileSystem::getInstance()->getFileName(outputImageFileName),
			false,
			false
		);
	} catch (Exception& exception) {
		Console::println("An error occurred: " + string(exception.what()));
	}
}
