#set text(font: ("linux libertine", "Source Han Serif SC", "Source Han Serif"))

#align(right+horizon)[
  #image("imgs/front.png")

  = MMD / Unknown Mother Goose / 卡通渲染测试

  2024 春 计算机图形学大作业报告

  *小组成员*

  08 马天开

  06 陈冠宇
]

#pagebreak()

== 简介

本次大作业以歌曲「Unknown Mother Goose」作为背景音乐, 利用 Blender、MMD、Davinci 等工具完成了一个 MMD 视频 (一类 3D 动画) 的制作, 我们讨论并分析卡通渲染的一些方法, 并在制作的过程中对使用的工具进行一定程度的改进.

== 背景

本次作业中, 人物模型、动作以及镜头都来自 MMD 社区, 并且以 `.pmx, .vmd` 等格式分发, 所以首要的任务就是如何将这些模型导入到 Blender 中进行渲染.

=== MMD

MMD <#link("https://en.wikipedia.org/wiki/MikuMikuDance")> 是一款由 Yu Higuchi 制作的免费 3D 动画制作软件, 尽管在 2011 年, MMD 的作者宣布停止软件的维护, 但 MMD 以其简单易用、社区活跃等特点仍然受到广泛欢迎.

在 2024 年, 我们不得不注意到 MMD 软件存在的一些问题:

- 与其他软件兼容性问题:

  MMD 为了方便用户使用, 集成了模型、动作、渲染等功能于一体, 但是对于想要做出更加细致调节的用户来说, 唯一的视频导出的选项显得不是那么友好.

- 平台兼容性问题:

  MMD 依赖 DirectX 9 API, 这使得其只能运行在 Windows 平台上, 并且随着 Windows 更新, DirectX 早期版本的支持也将成为问题.

针对以上问题, MMD 社区也对应的解决方案:

- 活跃于社区的 MMD 替代品: MMM <#link("https://sites.google.com/site/mikumikumovingeng")>, 最新版本发布于 2018 年.

  同样只支持 Windows 平台.

- MMD 导出 Alembic 文件的插件: MMDBridege <#link("https://github.com/uimac/mmdbridge")>, 由于是对 MMD 本体的插件, 并没有持续的维护.

- Blender mmd_tools 插件 <#link("https://github.com/UuuNyaa/blender_mmd_tools")>, 不再依赖 MMD 本体, 而是直接使用 Blender 导入对应模型和动作.

  Blender 计算物理并不会特殊处理人物模型, 比如在处理人物模型时我们经常会「砍手」来确保手部不会穿过身体, 这些问题都需要额外的处理. Blender 并不会自动处理这些问题.

=== Alembic

Alembic <#link("https://en.wikipedia.org/wiki/Alembic_(computer_graphics)")> 是一种用于交换复杂动画数据的开放格式, 它的特点是可以存储大量的顶点、法线、纹理等数据.

在 2024 年, Alembic 格式已经被广泛使用, 例如在电影制作中, 动画片制作中, 甚至是游戏开发中都可以看到 Alembic 格式的身影.

#pagebreak(weak: true)

== 任务

=== Stage 1.物理解算

我们小组共讨论了一些导入 `.pmx` 文件, 解算物理的方式, 以下是两个有望做出改进的方向:

+ Nanoem <#link("https://github.com/hkrn/nanoem")> Nanoem 是一款开源 (MIT License) 的 MMD 替代品, 虽然功能不尽相同, 但是作者实现了大部分 MMD 本体的功能, 并且项目代码开源, 可以在此基础上添加导出 Alembic 文件的功能, 并且适当改进其物理碰撞的算法

  #align(center)[
    #image("imgs/nanoem.png", width: 80%)
  ]

+ 改进 mmd_tools 插件对 Blender 4.1 的支持, 具体表现为在 Blender 4.1 上烘焙物理时容易发生内存溢出的问题, 读写文件、修改动作时容易出现错误

=== Stage 2. 卡通渲染

接下来是本次作业的另一个亮点, 如何在 Blender 中实现卡通渲染的效果, 我们讨论了以下几个值得回答的问题:

- 卡通勾线

  - 不同方式的勾线有什么优缺点? 性能如何? 效果如何?

  - 如何在 Blender 中实现这些效果?

- 卡通着色

  - 卡通着色的原理是什么? 如何在 Blender 中实现?

- 动画效果

- 提高 Blender 渲染速度

=== Stage 3. 后期制作

一个优秀的 MMD 视频不仅仅是模型、动作的堆砌, 后期制作需要完成字幕, 特效, 调色等工作.

#pagebreak(weak: true)

== Stage 1. 物理解算

在深入调研 mmd_tools 的开发情况后, 我们意识到这些问题主要来自 Blender 4.1 上游而不是插件, 同时我们注意到标准 abc 流程对于效率的提升, 可以将前期物理、布料解算的结果预先处理, 再导入到 Blender 进行下一步操作. 同时也能对 MMD 模型做特殊的处理, 这一点我们在下文中会提到.

综合以上原因, 我们尝试对 Nanoem 做一点改进:

=== Nanoem 改进

+ Alembic 支持的部分主要参考了以下来源:

  - Alembic 代码库 <#link("https://github.com/alembic/alembic")>

  - MMD Bridge 导出部分

  经过上面的处理后, 我们能正确导入到 Blender, 但是仍然存在一些解算的问题:

  #align(center)[
    #image("imgs/bad_export.jpg", width: 40%)
  ]

+ 物理解算的部分就是 Homework 10 的进阶版本, 在这里不再赘述, 经过一些处理, 最终可以得到正确的解算结果:

  #align(center)[
    #image("imgs/good_export.jpg", width: 70%)
  ]

+ #link("https://www.bilibili.com/video/BV1PM4m1r7Nk?p=1")[这里] 存放着一个不含物理解算的结果(视频); #link("https://www.bilibili.com/video/BV1PM4m1r7Nk?p=2")[这里] 存放着正确导出物理解算的结果(视频).
+ 对 Nanoem 修改部分的代码已随报告打包, 代码不是很稳定, 推荐直接使用导出的 `.abc` 文件.
  // , 这些代码目前只适配 macOS 平台 (用到 Metal API), 今年晚些时候会整理成 PR 提交给上游.

#pagebreak(weak: true)

== Stage 2. 卡通渲染

=== Blender 导入

接下来是使用 Blender 进行渲染的一些过程. 经过多次试错, 我们推荐按照如下方式将上文的 `.abc` 文件导入到 Blender:

+ 使用已经停止支持的 Blender 2.9 版本导入 `.abc`, 依次选择 `File -> Import -> Alembic (.abc)`, 选择导入的文件即可, 由于 `.pmx` 标准问题, 导入时应该选择 `0.08` 的缩放比例.

  #align(center)[
    #image("imgs/blender-import-abc-menu.png", width: 40%)
    #image("imgs/blender-import-abc-window.png", width: 80%)
  ]

+ 按空格键播放动画, 确认导入的动画效果正确.

  #align(center)[
    #image("imgs/blender-import-abc-test.png")
  ]

+ 将导入的 mesh 合并到一个 Collection 下, 方便复用.

  #align(center)[
    #image("imgs/blender-import-abc-collection.png", width: 30%)
  ]

+ 导出的 `.abc` 文件不包含颜色信息, 但是我们的导出工具已经将材质贴图一并输出到 `out/` 目录下, 打开这个目录, 分别创建三个 shader, 对应三张材质贴图. UV 信息应该正确包含在 `.abc` 导出的结果中, 只需要将这些 shader 对应到 mesh 上即可.

  #align(center)[
    #image("imgs/blender-import-abc-shader.png")
  ]

=== 主场景

经过上面的操作之后, 我们得到了一个 `.blend` 文件, 包含物理解算之后得到的顶点运动数据等, 我们接下来使用 Blender 4.1 进行操作:

+ 追加人物模型到当前场景, 依次选择 `File -> Append -> Object -> Collection`, 选择导入的 Collection 即可.

  #align(center)[
    #image("imgs/blender-2nd-import.png", width: 30%)
  ]

+ 按空格键播放动画, 确认导入的动画效果正确.

+ 在此基础上添加场景, 确认相对位置正确.

  #align(center)[
    #image("imgs/blender-main-scene.png")
  ]

=== 卡通渲染管线

在实际制作这些管线之前, 我们应该了解卡通渲染的几个方向, 分别是: #highlight[卡通勾线], #highlight[卡通着色], 动画效果.

+ 卡通勾线

  业界目前的勾线大致分为四类, 几何勾线、资源勾线、屏幕空间勾线、Ray Tracing 勾线, 我们来观察其中的几种:

  + 几何勾线:

    #align(center)[
      #image("imgs/blender-gs-stroke-demo.png", width: 50%)
    ]

    大致思路: 使用法线将模型挤出一定距离, 用于勾线的渲染. 这种方法性能很好, 可以控制粗细 (进而可以设置勾线粗细的远近效果), 但是无法处理模型内部的勾线.

    在 Blender 中, 我们可以使用一个几何着色器来实现这一效果:

    #image("imgs/blender-gs-stroke.png")

    在挤出之后, 需要对模型做一些细微的设置:

    #image("imgs/blender-gs-stroke-main.png")

  + 资源勾线:

    大致思路: 在贴图中直接包含勾线线条, 但是这种方法对贴图精度高, 低精度贴图会导致锯齿效果, 而且无法无破坏的修复勾线, 毕竟这是贴图的一部分.

    事实上, 这次我们选用的模型, 由于实际上是游戏资源的拆包, 可以看出部分模型的勾线是直接包含在贴图中的.

    #align(center)[
      #image("imgs/blender-asset-stroke.png", width: 50%)
    ]

    对游戏开发而言, 额外的资产意味着额外的工作量, 但是为应对手机等环境, 这种方法仍然是一种有效的选择.

  + 菲涅耳勾线:

    大致思路: 在模型表面根据法线角度计算勾线, 这种方法可以很好的处理模型内部的勾线, 但是对于模型表面的勾线控制较差.

    #align(center)[
      #image("imgs/blender-fresnel-stroke-demo.png")
    ]

    在 Blender 中, 我们可以使用一个菲涅耳着色器来实现这一效果:

    #image("imgs/blender-fresnel-stroke.png")

    此方法的缺点是, 相机接近模型时, 由于精度问题, 会出现不同程度的渲染错误:

    #image("imgs/blender-fresnel-stroke-failure-demo.png")

  + 屏幕空间勾线:

    大致思路: 在屏幕空间中计算勾线, 一般可以用 深度 + 法线等数据进行边缘查找.

    Blender 中的 FreeStyle 是一种基于屏幕空间的勾线渲染方式:

    #align(center)[
      #image("imgs/blender-freestyle-enable.png", width: 30%)
    ]

+ 卡通着色

  卡通材质是一类抽象程度较高的表现方式, 从表面属性来说, 一般有着以下几种维度:

  - 高光
  - 亮部颜色
  - 阴影过渡
  - 阴影颜色

  首先我们来实现二分光照模型:

  #align(center)[
    #image("imgs/blender-toon-shading.png")
  ]

  接下来与贴图结合:

  #align(center)[
    #image("imgs/blender-toon-shading-processing.png")
  ]

  计算高光:

  #image("imgs/blender-highlight.png")

  #image("imgs/blender-highlight-2.png")

  边缘光的计算:

  #image("imgs/blender-edge-light.png")

  #image("imgs/blender-edge-light-2.png")

+ 动画效果

  部分三渲二效果为追求和手绘动画的一致性, 会进行抽帧等处理, 但是这种处理对于 MMD 动画来说, 会导致动作不连续, 所以我们不推荐这种处理方式.

  但是我们同样可以通过分别渲染场景和人物, 人物动画一拍二, 背景一拍一的方式来实现类似的效果.

结合上面的内容, 我们可以得到 Blender 渲染的效果:

#image("imgs/blender-output.png")

#pagebreak(weak: true)

== Stage 3. 后期制作

在完成了上述的步骤之后, 我们接下来需要完成一些后期制作的工作:

#image("imgs/davinci.png")

为了实现上面的遮罩效果, 我们需要额外在 Blender 中渲染一个只包含人物的 mask, 类似下图:

#image("imgs/mask.png")

在 Blender 中实现上述输出的一个技巧是, 关闭场景建模, 将雾场通道渲染到输出:

#align(center)[
  #image("imgs/fog.png", width: 30%)
]

在合成节点中输出:

#align(center)[
  #image("imgs/fog-node.png", width: 60%)
]

== 分工

- 马天开: 负责 Blender 渲染部分, 包括卡通勾线、卡通着色等效果的实现, 以及后期制作的部分.

- 陈冠宇: 负责前期调研, Nanoem 的改进, 包括 Alembic 文件的导出, 物理解算等部分.

== 感想

+ 通过本次作业, 小组成员首次接触了 MMD 制作的流程, 深入学习使用了 Blender 软件, 了解了卡通渲染的一些方法, 也对 Alembic 格式有了更深入的了解.

+ 小组成员的团队协作能力得到了锻炼, 尤其在分工合作、任务分配等方面.

== 不足

+ 由于时间关系, 我们没有对 Nanoem 的代码进行更深入的改进

+ Blender 素材整理较为混乱, 技巧性不足

+ 后期效果可以更加完善

== 致谢

特别感谢刘老师和助教们的指导和付出, 感谢 MMD 社区中分享模型、动作的作者们.