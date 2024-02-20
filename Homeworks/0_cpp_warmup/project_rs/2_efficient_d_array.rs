use core::fmt;

#[derive(Debug, Clone)]
pub struct DArray {
    data: Vec<f64>,
    size: usize,
    allocated: usize,
}

impl DArray {
    pub fn set_at(&mut self, index: usize, value: f64) {
        self.data[index] = value;
    }

    pub fn get_at(&self, index: usize) -> f64 {
        self.data[index]
    }

    pub fn push_back(&mut self, value: f64) {
        self.data.push(value);
        self.size += 1;
    }

    pub fn delete_at(&mut self, index: usize) {
        self.data.remove(index);
        self.size -= 1;
    }

    pub fn insert_at(&mut self, index: usize, value: f64) {
        self.data.insert(index, value);
        self.size += 1;
    }

    pub fn size(&self) -> usize {
        self.size
    }

    pub fn set_size(&mut self, size: usize) {
        match size.cmp(&self.size) {
            std::cmp::Ordering::Greater => {
                for _ in 0..(size - self.size) {
                    self.data.push(0.0);
                }
            }
            std::cmp::Ordering::Less => {
                for _ in 0..(self.size - size) {
                    self.data.pop();
                }
            }
            std::cmp::Ordering::Equal => {}
        }
    }
}

impl Default for DArray {
    fn default() -> Self {
        DArray {
            data: Vec::new(),
            size: 0,
            allocated: 0,
        }
    }
}

impl fmt::Display for DArray {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "{:?}", self.data)
    }
}

fn main() {
    let mut a = DArray::default();
    a.insert_at(0, 2.1);
    println!("{}", a);

    a.push_back(3.0);
    a.push_back(3.1);
    a.push_back(3.2);
    println!("{}", a);

    a.delete_at(0);
    println!("{}", a);
    a.insert_at(0, 4.1);
    println!("{}", a);

    let a_copy = a.clone();
    println!("{}", a_copy);

    let a_copy = a;
    println!("{}", a_copy);
    // println!("{}", a);

    let mut b = DArray::default();
    b.push_back(21.0);
    println!("{}", b);
    b.delete_at(0);
    println!("{}", b);
    b.push_back(22.0);
    println!("{}", b);
    b.set_size(5);
    println!("{}", b);

    let mut c = DArray::default();
    c.push_back('a' as u8 as f64);
    c.push_back('b' as u8 as f64);
    c.push_back('c' as u8 as f64);
    c.insert_at(0, 'd' as u8 as f64);
    println!("{}", c);
}
