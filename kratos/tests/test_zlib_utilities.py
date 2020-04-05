from __future__ import print_function, absolute_import, division

import KratosMultiphysics
import KratosMultiphysics.KratosUnittest as KratosUnittest

class TestZlibUtilities(KratosUnittest.TestCase):

    def test_slib_utilities(self):
        KratosMultiphysics.Logger.GetDefaultOutput().SetSeverity(KratosMultiphysics.Logger.Severity.WARNING)

        lorem_ipsum = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Etiam eu elit non risus iaculis vestibulum a quis risus. Vestibulum ipsum metus, vulputate eu interdum sit amet, pharetra vitae ligula. Donec at turpis ut velit tincidunt congue. Nullam id ligula arcu. Phasellus bibendum convallis iaculis. Phasellus consectetur nibh eget pellentesque rutrum. Nam iaculis ante sit amet nisl congue blandit. Aliquam eu tempus ipsum, et fringilla arcu. Proin id interdum neque. Pellentesque lobortis mollis libero.

        Pellentesque sollicitudin sed magna id condimentum. Nam at risus est. Mauris et mi et felis ultricies pharetra id vel quam. Cras ac placerat arcu, vel faucibus nisi. Aliquam tincidunt urna quis arcu tristique, at ultricies magna posuere. Etiam congue, urna eu suscipit convallis, diam enim rhoncus nulla, quis sollicitudin orci nulla quis purus. Mauris eget eros libero. Aenean ut nulla eget erat laoreet tempus. Praesent eu justo orci. Pellentesque felis metus, consectetur et neque eget, bibendum gravida leo. Donec cursus eros sit amet orci luctus dictum. Morbi ut felis suscipit, rutrum odio id, aliquam purus. Phasellus non lorem eget turpis volutpat porta eget aliquet erat. Nunc felis diam, pulvinar a magna vitae, blandit luctus felis.

        Aliquam tellus diam, ornare et lectus vel, commodo ultrices neque. In odio lorem, ornare at facilisis ac, cursus id urna. Maecenas non tristique nisl. Praesent pellentesque ante lacus, id ultrices risus consequat vitae. Nam sollicitudin nulla porttitor hendrerit faucibus. Nunc sit amet mollis nulla. Praesent quis rhoncus libero. Ut in rutrum sem. Duis condimentum elit at felis interdum, ut fringilla erat rhoncus. Pellentesque nisl eros, tempor id leo nec, laoreet suscipit odio. Praesent id ante ac enim varius mattis. Ut at tristique lorem, vel lacinia est. Nullam pharetra tristique fringilla. In mollis, libero vitae fringilla condimentum, lorem turpis scelerisque tellus, et cursus justo nunc nec mauris.

        Cras rutrum at quam vel egestas. Duis tincidunt orci sit amet condimentum rhoncus. Duis finibus libero nec facilisis ultricies. In sit amet nibh ac massa rutrum volutpat et sed sapien. Donec sagittis sodales pretium. Integer arcu lectus, semper sit amet sem non, varius accumsan sapien. Mauris porta sapien magna, non egestas dolor tempor at. Proin non tortor non libero aliquet efficitur vel eget ipsum. Donec pharetra lacus in orci vestibulum, vel dictum neque maximus.

        Vestibulum accumsan varius tincidunt. Quisque id augue rutrum, vestibulum metus nec, condimentum sapien. Integer nec cursus urna. Integer tincidunt, sem vel dignissim interdum, nunc dui sollicitudin elit, vitae venenatis nunc lectus sed est. Donec iaculis mauris in pretium ullamcorper. Curabitur non congue lacus. Etiam efficitur dictum mauris, rhoncus convallis sapien iaculis eget. Aliquam vel risus vel ligula faucibus eleifend. Integer at iaculis lorem. Aenean aliquam enim at gravida iaculis. Integer orci sapien, vulputate non justo interdum, consequat dictum lectus. Nulla lorem odio, interdum non rutrum a, rhoncus in leo. Nulla condimentum interdum risus, nec fringilla urna viverra vel. Suspendisse a suscipit mauris.

        Nulla vulputate vitae arcu eget pellentesque. Quisque purus sapien, rutrum ac facilisis sed, viverra nec magna. Sed ut enim vel turpis iaculis pulvinar sed nec massa. In vehicula velit ut sagittis lobortis. Duis vulputate dictum hendrerit. Fusce ultrices massa ac nibh dignissim, at aliquam turpis dapibus. Nullam lorem dui, porttitor eu neque vitae, lacinia placerat massa. Proin ullamcorper, tellus quis ultrices convallis, velit nulla fringilla lorem, at auctor risus odio quis mi. Etiam id mi in diam venenatis feugiat. Sed lacinia erat eros, ut volutpat libero blandit sit amet. Proin a vulputate erat. Praesent ultrices condimentum ornare.

        Integer eleifend nibh sit amet ipsum rhoncus, a mollis lacus suscipit. In quis facilisis velit. Morbi id convallis nulla, ut dapibus tortor. Pellentesque sollicitudin ullamcorper nisl eu convallis. Morbi non nunc vitae mi interdum molestie nec eget nisl. Nunc ut consectetur nisi. Quisque auctor eleifend eros, ut hendrerit sem vestibulum vitae. Phasellus et sapien ex. Mauris sit amet ante posuere, pulvinar neque sit amet, lacinia erat. Aenean sit amet ullamcorper arcu. Donec et purus eu turpis sodales lacinia at nec nunc.

        Nullam vitae turpis a ante mollis semper. Praesent dictum hendrerit nibh, ut tincidunt nulla mattis at. Nunc et sapien bibendum, rutrum massa vitae, lacinia ipsum. Ut at feugiat magna, eget pharetra erat. Donec arcu tellus, laoreet sed eros non, dignissim lobortis quam. Lorem ipsum dolor sit amet, consectetur adipiscing elit. Interdum et malesuada fames ac ante ipsum primis in faucibus. Ut sodales mollis magna, eu faucibus arcu consectetur eu."

        lorem_ipsum_compressed = KratosMultiphysics.ZlibUtilities.CompressString(lorem_ipsum)
        lorem_ipsum_decompressed = KratosMultiphysics.ZlibUtilities.DecompressString(lorem_ipsum_compressed)

        self.assertEqual(lorem_ipsum, lorem_ipsum_decompressed)

if __name__ == '__main__':
    KratosUnittest.main()

