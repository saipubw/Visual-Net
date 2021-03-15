## Visual Net

2021/03/16

已更新，修复了一些bug，添加了一些更友好的控制台输出，程序现在会自动删除缓存图片。默认情况下关闭所有imshow()调试语句

本次更新提供了一个标准测试数据，供测试：

测试图片：[测试图片下载地址](https://stuxmueducn-my.sharepoint.com/:i:/g/personal/21620172203354_stu_xmu_edu_cn/EUJqmLkuV15Kps-0evI3csYBec1kNvlAJ2Lbm9HwgKS8xg?e=yK9f30)

该图片经过编码后生成的视频：[下载地址](https://stuxmueducn-my.sharepoint.com/:v:/g/personal/21620172203354_stu_xmu_edu_cn/Eb-C2q71oNFAv2TPpAZA9OMBnVMXwwMQIIvy4hBjR8Q9aA?e=9vcXYA)

手机拍摄的视频（刻意抖动，位移，旋转，改变透视以测试系统稳定度）（4k 60fps Redmi K20Pro)：[下载地址](https://stuxmueducn-my.sharepoint.com/:v:/g/personal/21620172203354_stu_xmu_edu_cn/EZLAUawmSX1CrVvoNL5BSyIB_VGsHARAQwSQgnoxKEhpSA?e=x2PRtW)

----------------------------------------------------------------------------------------------------------------------

简单的注意事项：

1. 视频录制时应该保证完整的录制（从开始到结束），只录制一部分是无法解析的
2. 视频录制时一定要小心，不要让鼠标/播放器的提示文字遮挡住二维码
3. 在调试时，可以启用代码中的debug宏，这样在关键步骤时就会显示图片，方便debug
4. 一定要开启Release模式
5. 使用vcpkg能很方便的配置opencv的环境，无需自己设定，详见[vcpkg官方文档](https://github.com/microsoft/vcpkg/blob/master/README_zh_CN.md)
7. 注意相对目录下一定要包含ffmpeg.exe，否则会报错
8. 建议先测试自己显示器/手机传感器的残影现象。一般来说，假如显示屏刷新率/相机录制帧率是60fps，那么，建议二维码的刷新率一般应该小于20fps，建议设置为15fps。[测试链接](https://www.testufo.com/chase) 你也可以去该网站自行测试

-----------------------------------------------------------------------------------------------------------------------

简单的介绍这个项目的工作流程：

1. 程序将文件读取到内存中
2. 通过自己设定的108\*108的二维码编码（包含原始信息，二维码定位点，帧编号，校验码），将原始信息转换成自定义的编码格式
3. 通过opencv，生成图片
4. 通过ffmpeg，将生成的图片序列合成为视频
5. 通过手机拍摄视频（也可直接使用原始视频）
6. 利用ffmpeg，将视频解码为图片序列
7. 利用opencv读取图片，重定位为原始二维码图片
8. 解析二维码中的信息，利用校验码判断解析是否出错
9. 根据得到的信息得到帧编号，判断是否出现跳帧和重帧
10. 将每一帧得到的原始信息拼接在一起，生成完整的输出文件。

-----------------------------------------------------------------------------------------------------------------------

这是一个可见光信息传输软件，通过将文件编码为视频，利用录制视频的形式，在另外一个设备上对录制的视频解码，从而利用可见光传输信息。

这是厦门大学信息学院计算机网络的课程项目。

软件编码端和解码端合为一体。利用ffmpeg，将多帧图像合成为视频，并利用ffmpeg对录制的视频进行解码。项目使用opencv进行图像识别和图片编码解码。

软件设计上只能传输单文件，目前的传输速率达到0.146Mbps。

命令行参数：

File2Video： 源数据路径，输出视频路径（最好以.mp4为结尾），最长视频时长(单位ms)，传输帧率（可省略，建议不超过15fps）

Video2File： 源视频路径，输出数据路径（最好以.mp4为结尾）
