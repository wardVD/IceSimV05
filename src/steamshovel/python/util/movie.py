from __future__ import print_function
import sys
import os, os.path
import tempfile
import subprocess
from icecube.icetray import i3logging

ffmpeg_flags = ['-y', # erase old files
                # on ffmpeg v1.1.3, theses are required to create quicktime-compatible mp4
                '-vprofile', 'main',
                '-pix_fmt', 'yuv420p',
                # Be as close to lossless as possible for the 'main' x264 profile
                '-crf', '1',
                # noninteractive
                '-nostdin',
                '-tune', 'animation',
                '-preset', 'veryslow',
                '-framerate', '25',
                #'-v','debug',
               ]

avconv_flags = ['-y', # erase old files
                '-pix_fmt', 'yuv420p',
                '-r', '25',
                '-format', 'mp4',
               ]

mencoder_flags = ['-mf', 'fps=25',
                  '-ovc', 'lavc',
                  '-lavcopts', 'vcodec=mpeg4',
                  '-o',
                 ]

def getMovieEncoders():
    '''
    Try all supported movie encoders (ffmpeg, avconv, mencoder)
    and return a list of the working ones.
    '''

    rval = []
    for encoder in ('ffmpeg', 'avconv', 'mencoder'):
        tmpfile1 = tempfile.NamedTemporaryFile(suffix='.png')
        tmpfile2 = tempfile.NamedTemporaryFile(suffix='.mp4')
        if encoder == 'ffmpeg':
            cmd = ['ffmpeg'] + ffmpeg_flags + ['-version']
        if encoder == 'avconv':
            cmd = ['avconv'] + avconv_flags + ['-version']
        if encoder == 'mencoder':
            cmd = ['mencoder', 'mf://'  + tmpfile1.name] + mencoder_flags + [tmpfile2.name]
        i3logging.log_debug( 'testing encoder: ' + ' '.join(cmd) )
        try:
            proc = subprocess.Popen( cmd,
                                     stderr=subprocess.STDOUT,
                                     stdout=subprocess.PIPE )
            output, _ = proc.communicate()
            retcode = proc.wait()
            if retcode == 0: # encoder is available and accepts all arguments
                rval.append( encoder )
            else:
                i3logging.log_info( '{0} does not pass the test\n{1}\nOutput:\n{2}'
                                    .format(encoder, ' '.join(cmd), output) )
        except OSError as e:
            i3logging.log_debug( encoder + ' is not installed or not in PATH' )
    return rval


def tempImageDir():
    return tempfile.mkdtemp()


class AsyncMovie:
    output = ""

    def __init__( self, encoder, inpdir, outfile ):
        if encoder == 'ffmpeg':
            cmd = [ 'ffmpeg', '-i', '{0}/frame%08d.png'.format( inpdir ) ]
            cmd += ffmpeg_flags
        elif encoder == 'avconv':
            cmd = [ 'avconv', '-i', '{0}/frame%08d.png'.format( inpdir ) ]
            cmd += avconv_flags
        elif encoder == 'mencoder':           
            cmd = [ 'mencoder', 'mf://{0}/*.png'.format( inpdir ) ]
            cmd += mencoder_flags
        else:
            i3logging.log_fatal( "bad encoder argument: {0}".format( encoder )  )
        cmd += [ outfile ]
        i3logging.log_debug( 'Encoding cmdline: ' + ' '.join(cmd) )
        self.proc = subprocess.Popen( cmd )

    def stop( self ):
        return self.proc.terminate()

    def isDone( self ):
        return (self.proc.poll() is not None)

