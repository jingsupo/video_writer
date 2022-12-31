#include <filesystem>
#include <iostream>
#include <vector>

#include <argparse/argparse.hpp>
#include <opencv2/opencv.hpp>

namespace fs = std::filesystem;

//递归遍历目录
void recursive_dir(const fs::path& dir, std::vector<std::string>& files)
{
    fs::recursive_directory_iterator end;
    fs::recursive_directory_iterator pos(dir);
    for (; pos != end; ++pos)
    {
        files.push_back(pos->path().string());
    }
}

void write(std::string in_path, int fps, int width, int height)
{
    if (not fs::exists(in_path))
    {
        std::cout << "无法打开文件夹\n";
        return;
    }

    std::vector<std::string> files;
    recursive_dir(in_path, files);

    auto video = cv::VideoWriter("out.mp4", cv::VideoWriter::fourcc('m', 'p', '4', 'v'), fps, { width, height });

    for (auto& f : files)
    {
        cv::Mat img = cv::imread(f);
        video.write(img);
    }

    video.release();
}

int main(int argc, char* argv[])
{
    argparse::ArgumentParser parser("video_writer");

    parser.add_argument("dir")
        .help("输入图片目录");
    parser.add_argument("-F", "--fps")
        .help("输出视频帧率")
        .default_value(25)
        .scan<'i', int>();
    parser.add_argument("-W", "--width")
        .help("输出视频帧宽度-必须与输入图片一致")
        .default_value(1280)
        .scan<'i', int>();
    parser.add_argument("-H", "--height")
        .help("输出视频帧高度-必须与输入图片一致")
        .default_value(720)
        .scan<'i', int>();

    try {
        parser.parse_args(argc, argv);
    }
    catch (const std::runtime_error& err) {
        std::cerr << err.what() << std::endl;
        std::cerr << parser;
        std::exit(1);
    }

    write(parser.get("dir"), parser.get<int>("fps"), parser.get<int>("width"), parser.get<int>("height"));
}