import os
import subprocess

def download(url,output=None,username=None,password=None):
    """Download a file to output (default: basename of url)"""
    if not output:
        output = os.path.basename(url)
    if username and password:
        wget_cmd = 'wget -q --tries=2 --user=%s --password=%s --output-document=%%s %%s'%(username,password)
        curl_cmd = 'curl -s --retry 2 -u %s:%s -o %%s %%s'%(username,password)
    else:
        wget_cmd = 'wget -q --tries=2 --output-document=%s %s'
        curl_cmd = 'curl -s --retry 2 -o %s %s'
    globus_cmd = 'globus-url-copy -cd -r -nodcau -rst-retries 5 -rst-interval 60 %s %s'
    cp_cmd = 'cp %s %s'
    
    if url.startswith('http://') or url.startswith('ftp://'):
        if subprocess.call(wget_cmd%(output,url),shell=True):
            if subprocess.call(curl_cmd%(output,url),shell=True):
                raise Exception('cannot download %s'%url)
    elif url.startswith('gsiftp://'):
        if subprocess.call(globus_cmd%(url,output),shell=True):
            raise Exception('cannot download %s'%url)
    elif url.startswith('file:/'):
        url = url[5:]
        if output.startswith('file:/'):
            output = output[5:]
        if subprocess.call(cp_cmd%(url,output),shell=True):
            raise Exception('cannot download %s'%url)
    elif url.startswith('/'):
        if subprocess.call(cp_cmd%(url,output),shell=True):
            raise Exception('cannot download %s'%url)
    else:
        raise Exception('unknown download protocol for %s'%url)

def isurl(url):
    for prefix in ['http://','https://','ftp://','gsiftp://','file:/']:
        if url.startswith(prefix):
            return True
    return False

def untar(path):
    if path.endswith('.tgz') or path.endswith('.tar.gz'):
        return not subprocess.call('tar -zxf %s'%(path,),shell=True)
    elif path.endswith('.tar.bz2'):
        return not subprocess.call('tar -jxf %s'%(path,),shell=True)
    elif path.endswith('.tar'):
        return not subprocess.call('tar -xf %s'%(path,),shell=True)
    elif path.endswith('.zip'):
        return not subprocess.call('unzip %s'%(path,),shell=True)
    else:
        raise Exception('unknown archive format')
