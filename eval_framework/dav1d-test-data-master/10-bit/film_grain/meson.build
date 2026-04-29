# Copyright © 2018, VideoLAN and dav1d authors
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
    [ 'av1-1-b10-23-film_grain-50', files('av1-1-b10-23-film_grain-50.ivf'), 'be596f5921854b9a9a5be81c302a5327' ],
    [ 'clip_0', files('clip_0.ivf'), '131f4465540a480824072b4f856c69ae' ],
    [ 'clip_1', files('clip_1.ivf'), '9e5a33d44ac655dde97598520e939631' ],
    [ 'clip_id', files('clip_id.ivf'), '59f3106a930a025a4d0fad1f4b532449' ],
    [ '422_oddheight', files('422_oddheight.ivf'), '2176d86c92104f3b8f93d08cb3c6bc84' ],
    [ '422_oddwidth', files('422_oddwidth.ivf'), 'f42cee4fe01a2c08bf2a00176547c3cc' ],
]

# test with film grain applied against dav1d's md5 
foreach test : fg_tests
    test(test[0], dav1d, suite: ['testdata-10', 'testdata'],
         args: dav1d_test_args + ['-i', test[1], '--filmgrain', '1', '--verify', test[2]])
endforeach
