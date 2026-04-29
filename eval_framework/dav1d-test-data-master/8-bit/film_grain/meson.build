# Copyright © 2018-2020, VideoLAN and dav1d authors
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice, this
#    list of conditions and the following disclaimer.
#
# 2. Redistributions in binary form must reproduce the above copyright notice,
#    this list of conditions and the following disclaimer in the documentation
#    and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
# ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

fg_tests = [
    [ 'av1-1-b8-23-film_grain-50', files('av1-1-b8-23-film_grain-50.ivf'), '392a4adc567fa05b210eebe15bcbb491' ],
    [ 'clip_0', files('clip_0.ivf'), '34277bfdff05f80c2435da591de562bd' ],
    [ 'clip_1', files('clip_1.ivf'), '1deabe78654f2bb06e9b44f54ea89995' ],
    [ 'clip_id', files('clip_id.ivf'), 'add701bf4a149bd69ed841499b98f9a0' ],
    [ '420_oddheight', files('420_oddheight.ivf'), '248d864983641a2af5962b7c209a5e55' ],
    [ '422_oddheight', files('422_oddheight.ivf'), '3e1df7404891ae3bbd439270fa4fac83' ],
    [ '422_oddwidth', files('422_oddwidth.ivf'), '84de4cff339522e8e25d429861189451' ],
]

# test with film grain applied against dav1d's md5
foreach test : fg_tests
    test(test[0], dav1d, suite: ['testdata-8', 'testdata'],
         args: dav1d_test_args + ['-i', test[1], '--filmgrain', '1', '--verify', test[2]])
endforeach
