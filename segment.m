for i=1:3
	image = imread(['image' num2str(i) '.png']);


	% RGB -> Grey
	gray = @(matrix) matrix(:, :, 1) * 0.2126 + matrix(:, :, 2) * 0.7152 + matrix(:, :, 3) * 0.0722;
	image = gray(image);
	imwrite(image, ['image' num2str(i) '-filter0.png']);
	im0 = image;

	% Mark the dark parts (red) using the average luminosity:

	luminosity = mean(mean(image));
	image = image < luminosity * 0.52;
	imwrite(image, ['image' num2str(i) '-filter1.png']);

	% Histogram equalization (to get better contrasts)
	im3 = histeq(im0);
	imwrite(im3, ['image' num2str(i) '-filter3.png']);

	% Edge detect using convolution
	im2 = conv2(double(im3), double([0 1 0; 1 -4 1; 0 1 0]));
	imwrite(im2, ['image' num2str(i) '-filter2.png']);

	% Now use some morphological closing
	circle = strel('disk', 20);
	image = imopen(im0, circle);
	imwrite(image, ['image' num2str(i) '-filter4.png']);

	continue;

	% Red filter
	image = image(:, :, 1) >= 100 & image(:, :, 1) <= 167 ...
		& image(:, :, 2) <= 40 & image(:, :, 3) <= 40;


	imwrite(image, ['image' num2str(i) '-filter1.png']);

	% Now use some morphological closing
	circle = strel('disk', 20);
	image = imopen(image, circle);
	imwrite(image, ['image' num2str(i) '-filter2.png']);

	% Imagine eroding the circles to their bare minimum.
	% Then, we can find the clumps that are minimal.



	imwrite(image, ['image' num2str(i) '-processed.png']);
end
