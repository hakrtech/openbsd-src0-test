# $Id: Mod_EUCJP.pm,v 1.3 2004/08/09 18:09:10 millert Exp $
# This file is in euc-jp
package Mod_EUCJP;
use encoding "euc-jp";
sub new {
  my $class = shift;
  my $str = shift || qw/���ʸ����/;
  my $self = bless { 
      str => '',
  }, $class;
  $self->set($str);
  $self;
}
sub set {
  my ($self,$str) = @_;
  $self->{str} = $str;
  $self;
}
sub str { shift->{str}; }
sub put { print shift->{str}; }
1;
__END__
