#############################################################################
#  $Id: gendlib.pl,v 1.3 2003-04-23 22:21:35 achu Exp $
#############################################################################
#  Copyright (C) 2001-2002 The Regents of the University of California.
#  Produced at Lawrence Livermore National Laboratory (cf, DISCLAIMER).
#  Written by Jim Garlick <garlick@llnl.gov>.
#  UCRL-CODE-2003-004.
#  
#  This file is part of Genders, a cluster configuration database and
#  rdist preprocessor.
#  For details, see <http://www.llnl.gov/linux/genders/>.
#  
#  Genders is free software; you can redistribute it and/or modify it under
#  the terms of the GNU General Public License as published by the Free
#  Software Foundation; either version 2 of the License, or (at your option)
#  any later version.
#  
#  Genders is distributed in the hope that it will be useful, but WITHOUT ANY
#  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
#  FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
#  details.
#  
#  You should have received a copy of the GNU General Public License along
#  with Genders; if not, write to the Free Software Foundation, Inc.,
#  59 Temple Place, Suite 330, Boston, MA  02111-1307  USA.
#############################################################################

package Genders;

use strict;
use vars qw($included $debug $havePSSP $handle);
use vars qw($init_called $init_hname_called $hname);
use vars qw($altAttr $clusterAttr);
use vars qw($GENDERS_ERR_SUCCESS $GENDERS_ERR_NOTFOUND);

use Libgenders;

if (!$included) {
$included = 1;

##
## Package globals
##

$debug =                0;

# See note in to_altnames().
$havePSSP =             (-d "/usr/lpp/ssp");
if ($havePSSP) {
        require "/admin/lib/sdrlib.pl";
} 

$altAttr =              "altname";
$clusterAttr =          "cluster";

$hname =                "";     # short hostname

$init_called = 0;               # needed for self-initialization
$init_hname_called = 0;

$handle = 0;                    # genders handle

$GENDERS_ERR_SUCCESS  = Libgenders::constant("GENDERS_ERR_SUCCESS", 0);   
$GENDERS_ERR_NOTFOUND = Libgenders::constant("GENDERS_ERR_NOTFOUND", 0);   

##
## Subroutines
##

# initialize package
#       $path (IN)      [optional] path to genders file
#       $rv (RETURN)    0 on failure opening genders, 1 on success
sub init
{
    my (@alist, $attr, $node, $blob, $gendfile, $ret);
    if (@_) {
        $gendfile = $_[0];
    } else {    
        $gendfile = Libgenders::string_constant("DEFAULT_GENDERS_FILE");
    }
    
    if ($init_called) {
        # destroy previous handle

        Libgenders::genders_handle_destroy($handle);
        $handle = 0;  
          
        $init_called = 0;
    }
    
    $handle = Libgenders::genders_handle_create();
    if ($handle == 0) {
        $debug && print "Error, genders_handle_create()\n";
        return 0;
    }

    $ret = Libgenders::genders_load_data($handle, $gendfile);
    if ($ret == -1) {
        $debug && print "Error, genders_load_data()\n";
        Libgenders::genders_handle_destroy($handle);
        $handle = 0;  
        return 0;
    }
    
    # initialize hostname
    if (!$init_hname_called) {
        $ret = init_hname();
        if ($ret == 0) {
            Libgenders::genders_handle_destroy($handle);
            $handle = 0;  
            return 0;
        }
    }
    
    $init_called = 1;
    $debug && print("init called\n");
    
    return 1;
}

# cache the local hostname
sub init_hname
{
    # get 'my' hostname
    $hname = Libgenders::genders_getnodename($handle);
    if ($hname eq "") {
        print "Error, genders_getnodename()\n";
        return 0;
    }

    $init_hname_called = 1;
    $debug && print("init_hname called\n");
    
    return 1;
}


# determine if host has attribute
#       $attr (IN)      attribute
#       $node (IN)      [optional] hostname
#       $found (RETURN) 0 if not found, 1 if found
sub hasattr
{
    if (!$init_called) {
        init();
        if (!$init_called) {
            return 0;
        }
    }

    my $attr = shift(@_);
    my $node = (@_) ? shift(@_) : $hname;
    
    my ($ret);
    
    if (defined($attr)) {
        $ret = Libgenders::genders_testattr($handle, $attr, $node);
        if ($ret == -1) {
            $debug && print "Error, genders_testattr()\n";    
            return 0;
        } 
        else {
            return $ret;
        }
    }
        
    return 0;
}

# return value of attribute held by host
#       $attr (IN)      attribute
#       $node (IN)      [optional] hostname
#       $value (RETURN) value or null if (no value or node does not have attr)
sub getattrval
{
    if (!$init_called) {
        init();
        if (!$init_called) {
            return "";
        }
    }
    
    my $attr = shift(@_);
    my $node = (@_) ? shift(@_) : $hname;
    
    my ($val);
    
    if (defined($attr)) {
        $val = Libgenders::genders_getattrval($handle, $attr, $node);
        if ($val eq "" && 
            Libgenders::genders_errnum($handle) != $GENDERS_ERR_SUCCESS) {
            
            $debug && print "Error, genders_getattrval()\n";    
            return "";
        } 
        else {
            return $val;
        }
    }
    return "";
}

# get list of attributes held by node
#       $node (IN)      [optional] hostname
#       @attrs (RETURN) list of attributes
sub getattr
{
    if (!$init_called) {
        init();
        if (!$init_called) {
            return ();
        }
    }
    
    my $node = (@_) ? shift(@_) : $hname;
    
    my ($attrs, $temp);
        
    $temp = Libgenders::genders_getattr($handle, $node);
    
    if (@$temp == 0 &&
        Libgenders::genders_errnum($handle) != $GENDERS_ERR_SUCCESS) {

        $debug && print "Error, genders_getattr()\n";
        return ();
    }
    else {
        ($attrs) = @$temp;
        return @$attrs;
    }
}

# get all attributes in genders file
#       @attrs (RETURN) list of attributes
sub getallattr
{
    if (!$init_called) {
        init();
        if (!$init_called) {
            return ();
        }
    }
    
    my ($attr);

    $attr = Libgenders::genders_getattr_all($handle);
    if (@$attr == 0 &&
        Libgenders::genders_errnum($handle) != $GENDERS_ERR_SUCCESS) {

        $debug && print "Error, genders_getattr_all()\n";
        return ();
    }
    else {
        return @$attr;
    }
}

# get list of nodes that have attribute
#       $attr (IN)      attribute
#       @nodes (RETURN) list of nodes
sub getnode
{
    if (!$init_called) {
        init();
        if (!$init_called) {
            return ();
        }
    }

    my $attr = shift(@_);
    my (@attrtemp, $nodes);

    if (defined($attr)) {
        if ($attr =~ /=/) {
            #strip attribute name and value

            @attrtemp = split(/=/, $attr);
            if (@attrtemp != 2) {
                return ();
            }
            else {
                $nodes = Libgenders::genders_getnodes($handle, 
                                                      $attrtemp[0], 
                                                      $attrtemp[1]);
            }
        }
        else {
            $nodes = Libgenders::genders_getnodes($handle, $attr);
        }
    }
    else {
        $nodes = Libgenders::genders_getnodes($handle);
    }
        
    if (@$nodes == 0 &&
        Libgenders::genders_errnum($handle) != $GENDERS_ERR_SUCCESS) {

        $debug && print "Error, genders_getnodes()\n";
        return ();
    }
    else {
        return @$nodes;
    }
}

# Get a copy of hash of attributes -> node lists.  
#       \%nodes (OUT)   node hash
sub get_node_hash
{
    my ($nodes) = (@_);
    
    my (%nodes, $attrs, $attr, $attrnodes);
    
    if (!$init_called) {
        init();
        if (!$init_called) {
            %{$nodes} = ();
            return;
        }
    }
    
    # must construct hash
    $attrs = Libgenders::genders_getattr_all($handle);
    if (@$attrs == 0 &&
        Libgenders::genders_errnum($handle) != $GENDERS_ERR_SUCCESS) {
        
        $debug && print "Error, genders_getattr_all()\n";
        %nodes = ();
    }
    elsif (@$attrs == 0) {
        %nodes = ();
    }
    else {
        foreach $attr (@$attrs) {
            $attrnodes = Libgenders::genders_getnodes($handle, $attr);
            if (@$attrnodes == 0 &&
                Libgenders::genders_errnum($handle) != $GENDERS_ERR_SUCCESS) {
                
                $debug && print "Error, genders_getnodes()\n";
                %nodes = ();
                last;
            }
            else {
                @{$nodes{$attr}} = @$attrnodes;   
            }
        }
    }
    %{$nodes} = %nodes;
}

# initialize list of clusters
#       $path (IN)      [optional] path to clusters file
#       $rv (RETURN)    0 on failure opening attributes, 1 on success
sub init_clusters
{
    # clusters file now removed, just return 1
    return(1);
}

# get a copy of the list of clusters
#       $rv (RETURN)    "" on failure, cluster name on success
# - to remain backwards compatible, a list containing the 
#   cluster name will be returned, not just the cluster name.   
sub get_clusters
{
    if (!$init_called) {
        init();
        if (!$init_called) {
            return "";
        }
    }

    my (@cluster, $cluster);
    
    $cluster = Libgenders::genders_getattrval($handle, $clusterAttr);
    if ($cluster eq "" && 
        Libgenders::genders_errnum($handle) != $GENDERS_ERR_SUCCESS) {

        $debug && print "Error, genders_getattrval()\n";    
        return "";
    } 

    @cluster = ($cluster);
    return @cluster;
}


# evaluate expression involving genders attributes
# (assume any non-numeric token is a genders attribute;  turn this into
# a boolean variable with value assigned by hasattr(), then evaluate
# the expression and return the result)
#       $exp (IN)       expression to evaluate
#       $node (IN)      [optional] hostname, use local host if unspecified
#       $rv (RETURN)    result of expression evaluation
sub gendexp
{
    if (!$init_called) {
        init();
        if (!$init_called) {
            return 0;
        }
    }

    my $exp = shift(@_);
    my $node = (@_) ? shift(@_) : $hname; 
    
    my ($var, $pro, %vars, $ret);
    
    if (!$exp) {
        return $exp;
    }

    $pro = "";
    foreach $var (split(/[\!\+\-\*\/(\s\(\&\|)]+/, $exp)) {
        $var =~ s/\s+//g;
        next if (!$var || $var =~ /^[0-9]+$/);
        $ret = Libgenders::genders_testattr($handle, $var, $node);
        if ($ret == -1) {
            $debug && print "Error, genders_testattr()\n";    
            return 0;
        } 
        elsif ($ret) {
            $pro .= "my \$$var = 1; ";
        } else {
            $pro .= "my \$$var = 0; ";
        }
        $vars{$var}++;
    }
    foreach $var (keys %vars) {
        $exp =~ s/$var/\$$var/g;
    }
    if ($debug) {
        printf("evaluating { %s } for host %s\n", $pro . $exp, $node);
    }
    no strict;
    return(eval($pro . $exp));
    use strict;
}

# Convert "genders names" to an alternate names.  On an SP, the genders
# name is assumed to be the SDR initial_hostname, and the alternate name
# is the reliable_hostname.  On other systems, the alternate hostname is
# stored in the genders for each node as the value of the "altname" attribute.
#       @inList (IN)    list of genders names
#       RETURN          list of alternate names
# NOTE: names in the input that cannot be converted are preserved in the output
sub to_altnames
{
    if (!$init_called) {
        init();
        if (!$init_called) {
            return ();
        }
    }
    
    my (@inList) = @_;
    my (@outList, $altName, $name);
    
    foreach $name (@inList) {
        ($name) = split(/\./, $name);   # shorten name
        if ($havePSSP) {
            $altName = Sdr::nn2ename(Sdr::sname2nn($name));
        } else {
            $altName = Libgenders::genders_getattrval($handle, $altAttr, $name);
            if ($altName eq "" && 
                Libgenders::genders_errnum($handle) != $GENDERS_ERR_SUCCESS &&
                Libgenders::genders_errnum($handle) != $GENDERS_ERR_NOTFOUND) {

                $debug && print "Error, genders_getattrval()\n";    
                return ();
            } 
            
        }
        push(@outList, $altName ? $altName : $name);
    }   
    return(@outList);
}

# perform the inverse of to_altnames()
#       @inList (IN)    list of altnernate names
#       RETURN          list of genders names
# NOTE: names in the input that cannot be converted are preserved in the output
sub to_gendnames
{
    if (!$init_called) {
        init();
        if (!$init_called) {
            return ();
        }
    }

    my (@inList) = @_;
    my (@outList, $altName, $name, $tmp, $nodes, $val);

    foreach $altName (@inList) {
        $name = "";
        ($altName) = split(/\./, $altName);     # shorten name
        if ($havePSSP) {
            $name = Sdr::nn2sname(Sdr::ename2nn($altName));
        } else {
            $nodes = Libgenders::genders_getnodes($handle, $altAttr);
            if (@$nodes == 0 &&
                Libgenders::genders_errnum($handle) != $GENDERS_ERR_SUCCESS) {
                $debug && print "Error, genders_getnodes()\n";    
                return ();
            }

            foreach $tmp (@$nodes) {
                $val = Libgenders::genders_getattrval($handle, $altAttr, $tmp);
                if ($val eq "" && 
                    Libgenders::genders_errnum($handle) != $GENDERS_ERR_SUCCESS) {
                    $debug && print "Error, genders_getattrval()\n";    
                    return ();
                } 
                
                if ($val eq $altName) {
                    $name = $tmp;
                    last;
                }
            }
        }
        push(@outList, $name ? $name : $altName);
    }   
    return(@outList);
}

}       # $included
1;      # return a true value...